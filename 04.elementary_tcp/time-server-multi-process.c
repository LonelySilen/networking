
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

const uint16_t kServerPort = 9090;
const char kBackLogSize = 128;

int main(void) {
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_ip;
  memset(&server_ip, 0, sizeof(server_ip));
  server_ip.sin_family = AF_INET;
  //server_ip.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  server_ip.sin_addr.s_addr = htonl(INADDR_ANY);
  server_ip.sin_port = htons(kServerPort);

  if (bind(sock_fd, (const struct sockaddr*) &server_ip,
           sizeof(server_ip)) == -1) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  if (listen(sock_fd, kBackLogSize) == -1) {
    perror("listen()");
    exit(EXIT_FAILURE);
  }
  for ( ; ; ) {
    struct sockaddr_in client_ip;
    socklen_t client_ip_len;
    int connect_fd = accept(sock_fd, (struct sockaddr*) &client_ip,
                            &client_ip_len);
    printf("Incoming connection");
    if (connect_fd < 0) {
      perror("accept()");
      continue;
    }
    printf("Client: %s:%d\n", inet_ntoa(client_ip.sin_addr),
           ntohs(client_ip.sin_port));

    switch (fork()) {
      case -1: {
        // Error occurs, do not handle current connection.
        perror("fork()");
        break;    
      }
      case 0: {
        // child process
        printf("[child: %ld] process created\n", getpid());
        fflush(stdout);
        const time_t ticks = time(NULL);
        char buff[64];
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connect_fd, buff, strlen(buff));
        sleep(3);
        close(connect_fd);
        printf("[child: %ld] close connection\n");
        fflush(stdout);
        _exit(0);
        break;
      }
      default: {
        // parent process
        break;
      }
    }
    // Close connection socket in parent process.
    close(connect_fd);
  }

  exit(EXIT_SUCCESS);
}
