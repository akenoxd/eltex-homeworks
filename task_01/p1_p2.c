#include <stdio.h>

int main() {
  int num;

  if (scanf("%d", &num) != 1) {
    printf("Invalid input\n");
    return 1;
  }

  for (int i = 31; i >= 0; i--) {
    printf("%d", (num >> i) & 1);

    if (i % 8 == 0)
      printf(" ");
  }
  printf("\n");

  return 0;
}