#include <stdio.h>

int main() {
  int arr[10];
  int *ptr = arr;

  for (int i = 0; i < 10; i++) {
    *(ptr + i) = i + 1;
    printf("%d ", *(ptr + i));
  }
  printf("\n");

  return 0;
}