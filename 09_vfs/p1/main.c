#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  long filesize = ftell(fp);
  if (filesize < 0) {
    printf("ftell error\n");
    fclose(fp);
    return 1;
  }
  char *buf = malloc(filesize + 1);
  if (!buf) {
    printf("malloc error");
    fclose(fp);
    return 1;
  }

  fseek(fp, 0, SEEK_SET);
  fread(buf, 1, filesize, fp);
  buf[filesize] = '\0';
  fclose(fp);

  // Print string in reverse
  for (long i = filesize - 1; i >= 0; --i) {
    putchar(buf[i]);
  }
  putchar('\n');
  free(buf);
  return 0;
}