#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int pipefd[2];
  pid_t pid;
  char buffer[100];

  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid > 0) { // parent

    close(pipefd[0]);

    const char *message = "Hi!";
    write(pipefd[1], message, strlen(message) + 1);

    close(pipefd[1]);

    waitpid(pid, NULL, 0);
  } else { // child
    close(pipefd[1]);

    read(pipefd[0], buffer, sizeof(buffer));

    printf("Received message: %s\n", buffer);

    close(pipefd[0]);

    exit(EXIT_SUCCESS);
  }

  return 0;
}