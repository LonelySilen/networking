
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

int main(void) {
  struct in_addr ip;
  ip.s_addr = 0x123456;

  printf("IP: %s\n", inet_ntoa(ip));

  return 0;
}
