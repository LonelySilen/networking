
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

// Exit infinite accept-loop when Ctrl+C is pressed.
int g_interupted = 0;

// Close socket and all children.
int g_terminated = 0;

void SigInt(int signo) {
  (void) signo;
  g_interupted = 1;
}

void SigTerm(int signo) {
  (void) signo;
  g_terminated = 1;
}

void SigChild(int signo) {
  (void) signo;

  pid_t pid;
  int stat;
  // Call waitpid() within a loop, fetching the status of any of children
  // that have terminated.
  // WNOHANG tell waitpid() not to block if there are running children
  // that have not yet terminated.
  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
    // TODO(LiuLang): Do not print message in signal handler.
    fprintf(stdout, "[parent] child %d terminated\n", pid);
    fflush(stdout);
  }
}

ssize_t Writen(int fd, const char* vptr, size_t n) {
  size_t nleft;
  nleft = n;
  while (nleft > 0) {
    const size_t nwritten = write(fd, vptr, nleft);
    if (nwritten <= 0) {
      // error occur
      return nwritten;
    }

    vptr += nwritten;
    nleft -= nwritten;
  }
  return n;
}

void StrEcho(int fd) {
  const size_t kBufSize = 128;
  char buf[kBufSize];

  ssize_t n;

  for ( ; ; ) {
    n = read(fd, buf, kBufSize);
    if (n < 0) {
      perror("read()");
      break;
    }

    if (n == 0) {
      // EOF
      break;
    }

    printf("[child: %d] received: %s\n", getpid(), buf);
    fflush(stdout);

    long arg1, arg2;
    if (sscanf(buf, "%ld%ld", &arg1, &arg2) == 2) {
      printf("arg1: %ld, arg2: %ld\n", arg1, arg2);
      snprintf(buf, kBufSize, "%ld\n", arg1 + arg2);
    } else {
      snprintf(buf, kBufSize, "input error\n");
    }
    printf("[child: %d] send: %s\n", getpid(), buf);
    fflush(stdout);

    n = strlen(buf);
    Writen(fd, buf, n);
    memset(buf, 0, kBufSize);

    if (n < 0 && errno == EINTR) {
      // readd
      continue;
    }
    if (n < 0) {
      perror("write()");
      break;
    }
  }
}

void StartWorkerProcessor(int conn_fd) {
  printf("[child: %d] inited\n", getpid());
  fflush(stdout);

  StrEcho(conn_fd);
  close(conn_fd);

  printf("[child: %d] terminated\n", getpid());
  fflush(stdout);

  _exit(EXIT_SUCCESS);
}

int main(void) {
  int sock_fd;

  struct sockaddr_in server_ip;
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }
  memset(&server_ip, 0, sizeof(server_ip));
  server_ip.sin_family = AF_INET;
  server_ip.sin_port = ntohs(kServerPort);
  server_ip.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sock_fd, (const struct sockaddr*)&server_ip,
        sizeof(server_ip)) < 0) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  const int kBackLog = 128;
  if (listen(sock_fd, kBackLog) < 0) {
    perror("listen()");
    exit(EXIT_FAILURE);
  }

  // Release children.
  BindSignal(SIGCHLD, SigChild, 1);

  BindSignal(SIGTERM, SigTerm, 0);

  // Update flag on Ctrl+C pressed.
  BindSignal(SIGINT, SigInt, 0);

  printf("[parent] %d\n", getpid());

  for ( ; ; ) {
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int conn_fd = accept(sock_fd, (struct sockaddr*) &client_addr, &client_len);
    printf("conn_fd: %d\n", conn_fd);
    if (conn_fd < 0) {
      printf("errno: %d\n", errno);
      if (g_terminated != 0) {
        perror("accept() terminated");
        break;
      }
      if (g_interupted != 0) {
        perror("accept() interrupted by Ctrl+C");
        break;
      }
      if (errno == EINTR) {
        // accept() was interrupted.
        perror("accept() was interrupted()");
      } else if (errno == ECONNABORTED) {
        perror("accept() Got connection aborted");
        // RST was received before accept()
      } else {
        perror("accept()");
      }
      continue;
    }

    switch (fork()) {
      case -1: {
        // error occurs
        perror("fork()");
        break;
      }
      case 0: {
        // child process
        StartWorkerProcessor(conn_fd);
      }
      default: {
        // parent process
        break;
      }
    }
    close(conn_fd);
  }

  close(sock_fd);

  exit(EXIT_SUCCESS);
}
