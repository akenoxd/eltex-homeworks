#include <stdio.h>
#include <stdlib.h>

int main() {
  int size = 0;

  printf("\ntask 2:\n");
  printf("input the size of the array: ");

  if (scanf("%d", &size) != 1 || size < 1) {
    printf("Invalid input\n");
    return 1;
  }

  int *array = (int *)malloc(size * sizeof(int));
  if (!array) {
    printf("malloc failed\n");
    return 1;
  }

  printf("input elements of the array: ");
  for (int i = 0; i < size; i++) scanf("%d", &array[i]);

  for (int i = size - 1; i >= 0; i--) printf("%d ", array[i]);
  printf("\n");

  free(array);
  return 0;
}