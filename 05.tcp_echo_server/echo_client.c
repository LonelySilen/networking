
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const uint16_t kServerPort = 9001;

void StrCli(FILE* input_file, int sock_fd) {
  const size_t kBufSize = 128;
  char send_line[kBufSize], recv_line[kBufSize];
  while (fgets(send_line, kBufSize, input_file) != NULL) {
    write(sock_fd, send_line, strlen(send_line));
    if (read(sock_fd, recv_line, kBufSize) <= 0) {
      perror("read()");
      return;
    }

    fputs(recv_line, stdout);
    memset(send_line, 0, kBufSize);
    memset(recv_line, 0, kBufSize);
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s ip-address\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_ip;
  memset(&server_ip, 0, sizeof(server_ip));
  server_ip.sin_family = AF_INET;
  server_ip.sin_port = htons(kServerPort);
  inet_pton(AF_INET, argv[1], &server_ip.sin_addr);

  const int kClients = 10;
  int sock_fds[kClients];
  for (int i = 0; i < kClients; i++) {
    sock_fds[i] = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fds[i] < 0) {
      perror("socket()");
      exit(EXIT_FAILURE);
    }

    if (connect(sock_fds[i], (const struct sockaddr*) &server_ip,
          sizeof(server_ip)) < 0) {
      perror("connect()");
      exit(EXIT_FAILURE);
    }
  }

  StrCli(stdin, sock_fds[0]);

  exit(EXIT_SUCCESS);
}
