#include <stdio.h>
#include <stdlib.h>

int main() {
  const char *filename = "output.txt";
  const char *text = "String from file";
  FILE *fp = fopen(filename, "w");
  if (!fp) {
    printf("write error\n");
    return 1;
  }

  fputs(text, fp);
  fclose(fp);

  fp = fopen(filename, "r");
  if (!fp) {
    printf("read error\n");
    return 1;
  }

  fseek(fp, 0, SEEK_END);
  int filesize = ftell(fp);
  if (filesize < 0) {
    printf("ftell error\n");
    fclose(fp);
    return 1;
  }

  for (int i = filesize - 1; i >= 0; i--) {
    fseek(fp, i, SEEK_SET);
    putchar(fgetc(fp));
  }
  putchar('\n');

  fclose(fp);
  return 0;
}