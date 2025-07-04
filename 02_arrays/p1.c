#include <stdio.h>
#include <stdlib.h>

int main() {
  int size = 0;

  printf("\ntask 1:\n");
  printf("input the size of the matrix: ");

  if (scanf("%d", &size) != 1 || size < 1) {
    printf("Invalid input\n");
    return 1;
  }

  int **matrix = malloc(size * sizeof(int *));
  if (matrix == NULL) {
    printf("malloc failed\n");
    return 1;
  }
  for (int i = 0; i < size; i++) {
    matrix[i] = malloc(size * sizeof(int));
    if (matrix[i] == NULL) {
      printf("malloc failed\n");
      for (int j = 0; j < i; j++) free(matrix[j]);
      free(matrix);
      return 1;
    }
  }

  int n = 1;
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++) matrix[i][j] = n++;

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