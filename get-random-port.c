
#include <stdio.h>
#include <sys/socket.h>

int main(void) {
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("Failed to call socket()");
    return 1;
  }

  const int kBacklog = 64;
  if (listen(sock_fd, kBacklog) != 0) {
    perror("Failed to call listen()");
    return 2;
  }

  struct sockaddr addr;
  socklen_t addr_len;
  if (getsockname(sock_fd, &addr, &addr_len) != 0) {
    perror("Failed to call getsockname()");
    return 3;
  }

  return 0;
}
