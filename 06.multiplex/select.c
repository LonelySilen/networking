
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const uint16_t kServerPort = 12345;

static int create_socket_server() {
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd == -1) {
    perror("socket()");
    return -1;
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(kServerPort);
  server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  int state = bind(sock_fd, (const struct sockaddr*) &server_addr,
                   sizeof(server_addr));
  if (state == -1) {
    perror("bind()");
    return -1;
  }

  return sock_fd;
}

int main(void) {
  int sock_fd = create_socket_server();
  if (sock_fd == -1) {
    exit(EXIT_FAILURE);
  }

  fd_set read_set;
  FD_ZERO(&read_set);
  FD_SET(STDIN_FILENO, &read_set);

  int n_fds = 1;
  int state = select(n_fds, &read_set, NULL, NULL, NULL);
  if (state == -1) {
    exit(EXIT_FAILURE);
  } else if (state == 0) {
    printf("No data to read\n");
  } else {
    printf("Data is availabel: %d\n", state);
    if (FD_ISSET(STDIN_FILENO, &read_set)) {
      printf("stdin got new data\n");
    }
  }

  close(sock_fd);
  exit(EXIT_SUCCESS);
}
