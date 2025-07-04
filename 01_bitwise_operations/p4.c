// #include <limits.h>
#include <stdio.h>

int main() {
  int num;
  int byte;

  printf("\ntask 4:\n");
  printf("input a number: ");

  if (scanf("%d", &num) != 1) {
    printf("Invalid input\n");
    return 1;
  }

  printf("input a number (0 - 255): ");

  if ((scanf("%d", &byte) != 1) && (byte < 0 || byte > 255)) {
    printf("Invalid input\n");
    return 1;
  }

  num = num & 0xff00ffff;
  num = num | (byte << 16);

  for (int i = 31; i >= 0; i--) {
    printf("%d", (num >> i) & 1);
    if (i % 8 == 0) printf(" ");
  }
  printf("\n");

  return 0;
}