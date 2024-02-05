#include <stdio.h>

int main() {
  struct xx {
    union {
      int x;
      char y;
    } u;

    char a[3];
  };

  struct xx x;
  x.u.x = 4;
  x.u.y = 'a';

  printf("u.x: %d, u.y: %c", x.u.x, x.u.y);

  return 0;
}
