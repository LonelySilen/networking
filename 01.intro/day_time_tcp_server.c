// Copyright (c) 2016 LiuLang. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "day_time_tcp.h"

int main(int argc, char** argv) {
  char buf[MAX_LINE];

  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    fprintf(stderr, "Failed to open socket()\n");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_address;
  bzero(&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(SERVER_PORT);

  if (bind(listen_fd,
           (const struct sockaddr*) &server_address,
           sizeof(server_address)) < 0) {
    fprintf(stderr, "Failed to bind server\n");
    exit(EXIT_FAILURE);
  }

  if (listen(listen_fd, SOCK_STREAM) < 0) {
    fprintf(stderr, "Failed to listen to incoming connection\n");
    exit(EXIT_FAILURE);
  }

  for ( ; ; ) {
    int connect_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
    if (connect_fd < 0) {
      fprintf(stderr, "accept() failed\n");
      break;
    }
    time_t ticks = time(NULL);
    snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
    write(connect_fd, buf, strlen(buf));

    fprintf(stdout, "Send replay: %s", buf);

    close(connect_fd);
  }
}