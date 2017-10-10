
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const uint16_t kServerPort = 12345;

void StrCli(FILE* input_file, int sock_fd) {
  const size_t kBufSize = 128;
  char send_line[kBufSize];
  while (fgets(send_line, kBufSize, input_file) != NULL) {
    const int state = write(sock_fd, send_line, strlen(send_line));
    if (state == -1) {
      perror("write()");
      return;
    }
    memset(send_line, 0, kBufSize);
  }
}

int main(void) {
  struct sockaddr_in server_ip;
  memset(&server_ip, 0, sizeof(server_ip));
  server_ip.sin_family = AF_INET;
  server_ip.sin_port = htons(kServerPort);
  server_ip.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  if (connect(sock_fd, (const struct sockaddr*) &server_ip,
      sizeof(server_ip)) < 0) {
    perror("connect()");
    exit(EXIT_FAILURE);
  }

  StrCli(stdin, sock_fd);
  close(sock_fd);

  exit(EXIT_SUCCESS);
}
