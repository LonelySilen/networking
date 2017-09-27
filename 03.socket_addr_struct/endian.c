
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  union checker_t {
    short s;
    char c[sizeof(short)];
  };

  union checker_t checker;
  checker.s = 0x0102;

  if (sizeof(short) == 2) {
    if (checker.c[0] == 1 && checker.c[1] == 2) {
      printf("Big endian\n");
    } else {
      printf("Little endian\n");
    }
  } else {
    printf("Sizeof(short): %d\n", sizeof(short));
  }

  exit(EXIT_SUCCESS);
}
