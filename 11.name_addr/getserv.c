
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
  struct servent* ent_ptr;

  for (int i = 1; i < argc; i++) {
    ent_ptr = getservbyname(argv[i], NULL);
    if (ent_ptr == NULL) {
      printf("Not found: %s\n", argv[i]);
    } else {
      printf("name: %s, port: %d, proto: %s",
          ent_ptr->s_name,
          ntohs(ent_ptr->s_port),
          ent_ptr->s_proto);
      char** alias = ent_ptr->s_aliases;
      printf(", aliases:");
      while (*alias != NULL) {
        printf(" %s", *alias);
        *alias ++;
      }
      printf("\n");
    }
  }

  exit(EXIT_SUCCESS);
}
