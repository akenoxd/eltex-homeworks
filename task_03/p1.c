#include <limits.h>
#include <stdio.h>

int main() {
  int num;
  int byte;
  if (scanf("%d", &num) != 1) {
    printf("Invalid input\n");
    return 1;
  }
  if (scanf("%d", &byte) != 1 && byte < 0 || byte > 255) {
    printf("Invalid input\n");
    return 1;
  }
  char *p = (char *)&num;
  p += 2;
  *p = byte;

  for (int i = 31; i >= 0; i--) {
    printf("%d", (num >> i) & 1);
    if (i % 8 == 0)
      printf(" ");
  }
  printf("\n");

  return 0;
}