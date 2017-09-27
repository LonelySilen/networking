
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const uint16_t kServerPort = 9090;

int main(void) {
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_ip;
  memset(&server_ip, 0, sizeof(server_ip));
  server_ip.sin_family = AF_INET;
  server_ip.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  server_ip.sin_port = htons(kServerPort);
  printf("address of server_ip: %ld\n", server_ip.sin_addr.s_addr);

  if (connect(sock_fd, (const struct sockaddr*) &server_ip,
              sizeof(server_ip)) == -1) {
    perror("connect()");
    exit(EXIT_FAILURE);
  }

  const size_t kBufSize = 64;
  char buf[kBufSize + 1];
  ssize_t num_read = read(sock_fd, buf, kBufSize);
  if (num_read < 0) {
    perror("read()");
    exit(EXIT_FAILURE);
  }
  buf[num_read] = '\0';
  printf("Received: %s, size: %ld\n", buf, num_read);

  close(sock_fd);

  exit(EXIT_SUCCESS);
}
