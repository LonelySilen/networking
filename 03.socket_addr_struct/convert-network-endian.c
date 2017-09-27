
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  const uint32_t host_ip = 0x0102;
  const uint32_t net_ip = htonl(host_ip);
  printf("host ip: %d, net ip: %d\n", host_ip, net_ip);

  exit(EXIT_SUCCESS);
}
