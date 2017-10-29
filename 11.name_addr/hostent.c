
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
  char* ptr, **pptr;
  char str[INET_ADDRSTRLEN];
  struct hostent* hptr;

  while ((--argc) > 0) {
    ptr = *(++argv);
    hptr = gethostbyname(ptr);
    if (hptr == NULL) {
      perror("gethostbyname()");
      continue;
    }

    printf("official hostname: %s\n", hptr->h_name);
    for (pptr = hptr->h_aliases; *pptr != NULL; pptr++) {
      printf("\talias: %s\n", *pptr);
    }

    switch (hptr->h_addrtype) {
      case AF_INET: {
        pptr = hptr->h_addr_list;
        for (; *pptr != NULL; pptr++) {
          inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str));
          printf("\taddress: %s\n", str);
        }
        break;
      }
      default: {
        fprintf(stderr, "unknown address type");
        break;
      }
    }
  }

  exit(EXIT_SUCCESS);
}
