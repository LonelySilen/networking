
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv) {
  struct addrinfo hints;
  struct addrinfo* result = NULL;
  struct addrinfo* ptr = NULL;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_CANONNAME;
  hints.ai_family = AF_INET;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s hostname service\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int status = getaddrinfo(argv[1], argv[2], &hints, &result);
  if (status != 0) {
    perror("getaddrinfo()");
    exit(EXIT_FAILURE);
  }

  const ssize_t kBufSize = 128;
  char buf[kBufSize];
  ptr = result;
  for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
    switch (ptr->ai_addr->sa_family) {
      case AF_INET: {
        struct sockaddr_in* in = (struct sockaddr_in*) ptr->ai_addr;
        if (inet_ntop(AF_INET, &in->sin_addr, buf, kBufSize) != NULL) {
          printf("%s:%d\n", buf, ntohs(in->sin_port));
        }
        break;
      }
      case AF_INET6: {
        struct sockaddr_in6* in = (struct sockaddr_in6*) ptr->ai_addr;
        if (inet_ntop(AF_INET6, &in->sin6_addr, buf, kBufSize) != NULL) {
          printf("%s:%d\n", buf, ntohs(in->sin6_port));
        }
        break;
      }
      default: {
        break;
      }
    }
  }
  free(result);

  exit(EXIT_SUCCESS);
}
