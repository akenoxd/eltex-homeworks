#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  char command[1025];
  char line[129];
  while (1) {
    printf("> ");
    fgets(command, 1024, stdin);
    *strchr(command, '\n') = '\0';

    if (strcmp(command, "exit") == 0) break;

    FILE *output = popen(command, "r");
    while (fgets(line, 128, output)) printf("%s", line);

    pclose(output);
  }

  exit(EXIT_SUCCESS);
}