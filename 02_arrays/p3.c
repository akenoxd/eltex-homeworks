#include <stdio.h>
#include <stdlib.h>

int main() {
  int size = 0;

  printf("\ntask 2:\n");
  printf("input the size of matrix: ");

  if (scanf("%d", &size) != 1) {
    printf("Invalid input\n");
    return 1;
  }

  int **matrix = malloc(size * sizeof(int *));
  if (matrix == NULL) {
    printf("Memory allocation failed\n");
    return 1;
  }
  for (int i = 0; i < size; i++) {
    matrix[i] = malloc(size * sizeof(int));
    if (matrix[i] == NULL) {
      printf("Memory allocation failed\n");
      for (int j = 0; j < i; j++) free(matrix[j]);
      free(matrix);
      return 1;
    }
  }

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

  for (int i = 0; i < size; i++) free(matrix[i]);
  free(matrix);

  return 0;
}
