// Copyright (c) 2017 LiuLang. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#include "udp_const.h"

static void dg_cli(FILE* input_fd, int sock_fd,
                   const struct sockaddr* server_addr, socklen_t addr_len) {
  int n;
  char send_msg[kMsgLine];
  char recv_msg[kMsgLine];

  while (fgets(send_msg, kMsgLine, input_fd) != NULL) {
    sendto(sock_fd, send_msg, strlen(send_msg), 0, server_addr, addr_len);
    n = recvfrom(sock_fd, recv_msg, kMsgLine, 0, NULL, NULL);
    recv_msg[n] = '\0';  // null terminate
    fputs(recv_msg, stdout);
  }
}

int main(void) {
  int sock_fd;
  struct sockaddr_in server_addr;

  memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(kServerPort);
  server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

  dg_cli(stdin, sock_fd, (const struct sockaddr*)&server_addr,
         sizeof(server_addr));

  return 0;
}