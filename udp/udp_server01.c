// Copyright (c) 2017 LiuLang. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "udp_const.h"

void dg_echo(int sock_fd, const struct sockaddr* client_addr,
             socklen_t addrlen) {
  int n;
  socklen_t len;
  char msg[kMsgLine];

  for ( ; ;) {
    len = addrlen;
    n = recvfrom(sock_fd, msg, kMsgLine, 0, client_addr, &len);
    sendto(sock_fd, msg, n, 0, client_addr, len);
  }
}

int main(int argc, char** argv) {
  int sock_fd;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;

  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    perror("socket");
    return 1;
  }

  memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(kServerPort);

  if (bind(sock_fd,
           (struct sockaddr *)&server_addr,
           sizeof(server_addr)) != 0) {
    perror("bind");
    return 2;
  }
  dg_echo(sock_fd, (const struct sockaddr*) & client_addr,
          sizeof(client_addr));

  return 0;
}