
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = 9090;

  const char kGoogDns[] = "8.8.8.8";
  if (inet_aton(kGoogDns, &addr.sin_addr) == 0) {
    perror("inet_aton()");
    exit(EXIT_FAILURE);
  }
  printf("Numberic ip of %s is %d\n", kGoogDns, addr.sin_addr.s_addr);

  return 0;
}
