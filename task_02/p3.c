#include <stdio.h>
#include <stdlib.h>

int main() {
  int size;

  if (scanf("%d", &size) != 1) {
    printf("Invalid input\n");
    return 1;
  }

  int **matrix = malloc(size * sizeof(int *));
  for (int i = 0; i < size; i++)
    matrix[i] = malloc(size * sizeof(int));

  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      if ((size - i - 1) > j)
        matrix[i][j] = 0;
      else
        matrix[i][j] = 1;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      printf("%d ", matrix[i][j]);
    }
    printf("\n");
  }

  for (int i = 0; i < size; i++)
    free(matrix[i]);
  free(matrix);

  return 0;
}
