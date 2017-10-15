
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

const uint16_t kServerPort = 9191;

static void Echo(int sock_fd, struct sockaddr* client_addr,
                 socklen_t client_len) {
  ssize_t n_recv;
  ssize_t n_send;
  socklen_t len;
  const ssize_t kMsgBuf = 1024;
  char buf[kMsgBuf];

  for ( ; ; ) {
    len = client_len;
    n_recv = recvfrom(sock_fd, buf, kMsgBuf, 0, client_addr, &len);
    printf("%s", buf);
    n_send = sendto(sock_fd, buf, n_recv, 0, client_addr, len);
    if (n_send == -1) {
      perror("sendto()");
      break;
    }
  }
}

int main(void) {
  int sock_fd;
  struct sockaddr_in server_addr, client_addr;

  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(kServerPort);

  int status = bind(sock_fd, (const struct sockaddr*) &server_addr,
                    sizeof(server_addr));
  if (status == -1) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  Echo(sock_fd, (struct sockaddr*) &client_addr, sizeof(client_addr));

  exit(EXIT_SUCCESS);
}
