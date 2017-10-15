
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const uint16_t kServerPort = 9191;

static void Echo(FILE* input, int sock_fd, struct sockaddr* server_addr,
                 socklen_t server_len) {
  ssize_t n_send = 0, n_recv = 0;
  const ssize_t kBufSize = 1024;
  char send_buf[kBufSize], recv_buf[kBufSize + 1];
  int status = 0;

  status  = connect(sock_fd, server_addr, server_len);
  if (status == -1) {
    perror("connect()");
    return;
  }

  while (fgets(send_buf, kBufSize, input) != NULL) {
    n_send = write(sock_fd, send_buf, strlen(send_buf));
    n_recv = read(sock_fd, recv_buf, kBufSize);

    if (n_send != n_recv) {
      fprintf(stderr, "send: %d, recv: %d not match\n", n_send, n_recv);
    }

    // Add null termination to end of string.
    recv_buf[n_recv] = '\0';

    fputs(recv_buf, stdout);
  }
}

int main(void) {
  int sock_fd;
  struct sockaddr_in server_addr;

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(kServerPort);

  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  Echo(stdin, sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));

  exit(EXIT_SUCCESS);
}
