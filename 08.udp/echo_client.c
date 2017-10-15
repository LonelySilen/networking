
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

const uint16_t kServerPort = 9191;

static void Echo(FILE* input, int sock_fd, struct sockaddr* server_addr,
                 socklen_t server_len) {
  ssize_t n_send, n_recv;
  const ssize_t kBufSize = 1024;
  char send_buf[kBufSize], recv_buf[kBufSize + 1];
  socklen_t reply_len;
  struct sockaddr* reply_addr = NULL;

  reply_addr = malloc(server_len);
  if (reply_addr == NULL) {
    perror("malloc()");
    return;
  }

  while (fgets(send_buf, kBufSize, input) != NULL) {
    n_send = sendto(sock_fd, send_buf, strlen(send_buf), 0, server_addr,
                    server_len);
    reply_len = server_len;
    n_recv = recvfrom(sock_fd, recv_buf, kBufSize, 0, reply_addr, &reply_len);

    if ((reply_len != server_len) ||
        (memcmp(server_addr, reply_addr, reply_len) != 0)) {
      char host_buf[128];
      const char* host_ptr = inet_ntop(AF_INET, (const void*) server_addr,
                                       host_buf, reply_len);
      fprintf(stderr, "reply from %s (ignored)\n", host_ptr);
      continue;
    }

    if (n_send != n_recv) {
      fprintf(stderr, "send: %d, recv: %d not match\n", n_send, n_recv);
    }

    // Add null termination to end of string.
    recv_buf[n_recv] = '\0';

    fputs(recv_buf, stdout);
  }
}

int main(void) {
  int sock_fd;
  struct sockaddr_in server_addr;

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(kServerPort);

  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  Echo(stdin, sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));

  exit(EXIT_SUCCESS);
}
