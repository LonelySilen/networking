
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  struct sockaddr_in ip4;
  memset(&ip4, 0, sizeof(ip4));
  const char kGoogDns[] = "8.8.8.8";
  const int ret = inet_pton(AF_INET, kGoogDns, &ip4.sin_addr);
  if (ret < 0) {
    perror("inet_pton()");
    return 1;
  }
  if (ret == 0) {
    printf("Not in presentation format\n");
    return 1;
  }
  printf("ip of google dns %s is %ld\n", kGoogDns, ip4.sin_addr.s_addr);

  return 0;
}
