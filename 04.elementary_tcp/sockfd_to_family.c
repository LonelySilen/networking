
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(void) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_storage ss;
  socklen_t len = sizeof(ss);

  if (getsockname(sockfd, (struct sockaddr*) &ss, &len) < 0) {
    perror("getsockname()");
    exit(EXIT_FAILURE);
  }

  switch (ss.ss_family) {
    case AF_INET: {
      struct sockaddr_in sock4;
      memcpy(&sock4, &ss, sizeof(sock4));
      printf("port: %d, addr: %s\n", sock4.sin_port,
             inet_ntoa(sock4.sin_addr));
      break;
    }
    case AF_INET6: {
      break;
    }
    default: {
      break;
    }
  }
  printf("socket family: %d, AF_INET: %d\n", ss.ss_family, AF_INET);

  exit(EXIT_SUCCESS);
}
