
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(void) {
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd == -1) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  const char kLocalHost[] = "127.0.0.1";

  struct sockaddr_in client_ip;
  memset(&client_ip, 0, sizeof(client_ip));
  client_ip.sin_family = AF_INET;
  client_ip.sin_port = htons(43110);
  if (inet_aton(kLocalHost, &client_ip.sin_addr) < 1) {
    perror("inet_aton(client_ip)");
    exit(EXIT_FAILURE);
  }
  printf("address of client_ip: %u\n", client_ip.sin_addr.s_addr);

  if (bind(sock_fd, (const struct sockaddr*) &client_ip,
           sizeof(client_ip)) == -1) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_ip;
  memset(&server_ip, 0, sizeof(server_ip));

  server_ip.sin_family = AF_INET;
  server_ip.sin_port = htons(9090);
  if (inet_aton(kLocalHost, &server_ip.sin_addr) < 1) {
    perror("inet_aton()");
    exit(EXIT_FAILURE);
  }
  printf("address of server_ip: %u\n", server_ip.sin_addr.s_addr);

  if (connect(sock_fd, (const struct sockaddr*) &server_ip,
              sizeof(server_ip)) == -1) {
    perror("connect()");
    switch (errno) {
      case ECONNREFUSED: {
        fprintf(stderr, "Connection refushed\n");
        break;
      }
      default: {
        break;
      }
    }

    exit(EXIT_FAILURE);
  }
  
  exit(EXIT_SUCCESS);
}
