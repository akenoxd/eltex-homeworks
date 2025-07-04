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
    for (int j = 0; j < size; j++) matrix[i][j] = 0;

  int dirs[4][2] = {
      {0, 1}, {1, 0}, {0, -1}, {-1, 0}};  // вправо, вниз, влево, вверх
  int dir = 0;
  int x = 0, y = 0;
  int num = 1;

  for (int k = 0; k < size * size; k++) {
    matrix[x][y] = num++;
    int nx = x + dirs[dir][0];
    int ny = y + dirs[dir][1];
    if (nx < 0 || nx >= size || ny < 0 || ny >= size || matrix[nx][ny] != 0) {
      dir = (dir + 1) % 4;
      nx = x + dirs[dir][0];
      ny = y + dirs[dir][1];
    }
    x = nx;
    y = ny;
  }

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      printf("%3d ", matrix[i][j]);
    }
    printf("\n");
  }

  for (int i = 0; i < size; i++) free(matrix[i]);
  free(matrix);

  return 0;
}
