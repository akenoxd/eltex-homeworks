#include <stdio.h>
#include <stdlib.h>

int main() {
  int size;
  if (scanf("%d", &size) != 1 || size < 1) {
    printf("Invalid input\n");
    return 1;
  }

  int *array = (int *)malloc(size * sizeof(int));
  for (int i = 0; i < size; i++)
    scanf("%d", &array[i]);

  for (int i = size - 1; i >= 0; i--)
    printf("%d ", array[i]);
  printf("\n");

  free(array);
  return 0;
}