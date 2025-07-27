#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 32

int main() {
  char command[1025];
  char *args[MAX_ARGS + 1]; // +1 для NULL в конце
  char *token;

  while (1) {
    printf("> ");
    fgets(command, 1024, stdin);
    command[strcspn(command, "\n")] = '\0';

    if (strcmp(command, "exit") == 0)
      break;

    int i = 0;
    token = strtok(command, " ");
    while (token != NULL && i < MAX_ARGS) {
      args[i++] = token;
      token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid == 0) {
      execvp(args[0], args);
      perror("execvp");
      exit(EXIT_FAILURE);
    } else if (pid > 0) {
      int status;
      waitpid(pid, &status, 0);
    } else {
      perror("fork");
    }
  }

  return EXIT_SUCCESS;
}