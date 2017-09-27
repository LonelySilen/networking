
#include <netinet/ip.h>
#include <stdio.h>

int main(void) {
  printf("Sizeof(sockaddr_in): %ld\n", sizeof(struct sockaddr_in));
  printf("Sizeof(sockaddr_in6): %ld\n", sizeof(struct sockaddr_in6));

  return 0;
}
