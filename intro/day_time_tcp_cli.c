// Copyright (c) 2016 LiuLang. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include <memory.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "day_time_tcp.h"

int main(int argc, char** argv) {
  int sock_fd;
  ssize_t n;
  char receive_line[MAX_LINE + 1];
  struct sockaddr_in server_address;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <IP address>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    fprintf(stderr, "socket error!\n");
    exit(EXIT_FAILURE);
  }

  bzero(&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(SERVER_PORT);
  if (inet_pton(AF_INET, argv[1], &server_address.sin_addr) <= 0) {
    fprintf(stderr, "inet_pton() error for %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  if (connect(sock_fd,
              (const struct sockaddr *) &server_address,
              sizeof(server_address)) < 0) {
    fprintf(stderr, "connect error!\n");
    exit(EXIT_FAILURE);
  }

  while ((n = read(sock_fd, receive_line, MAX_LINE)) > 0) {
    receive_line[n] = 0;  /* null terminate */
    if (fputs(receive_line, stdout) == EOF) {
      fprintf(stderr, "fputs error\n");
      exit(EXIT_FAILURE);
    }
  }

  if (n < 0) {
    fprintf(stderr, "read error\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}