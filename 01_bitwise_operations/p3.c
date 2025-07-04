#include <limits.h>
#include <stdio.h>

int main() {
  int num;

  printf("\ntask 3:\n");
  printf("input a number: ");

  if (scanf("%d", &num) != 1) {
    printf("Invalid input\n");
    return 1;
  }

  int count = 0;
  for (int i = 31; i >= 0; i--) {
    int bit = (num >> i) & 1;
    if (bit == 1)
      count++;
    printf("%d", bit);

    if (i % 8 == 0)
      printf(" ");
  }

  printf("\nnumber of ones: %d\n", count);

  return 0;
}