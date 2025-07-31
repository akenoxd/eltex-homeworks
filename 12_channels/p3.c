#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 32
#define MAX_COMMANDS 10

void execute_command(char *args[], int input_fd, int output_fd) {
  pid_t pid = fork();
  if (pid == 0) {

    if (input_fd != STDIN_FILENO) {
      dup2(input_fd, STDIN_FILENO);
      close(input_fd);
    }
    if (output_fd != STDOUT_FILENO) {
      dup2(output_fd, STDOUT_FILENO);
      close(output_fd);
    }

    execvp(args[0], args);
    perror("execvp");
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    ;
  } else if (pid < 0) {
    perror("fork");
  }
}

int main() {
  char command[1025];
  char *commands[MAX_COMMANDS][MAX_ARGS + 1];
  int num_commands = 0;

  while (1) {
    printf("> ");
    fgets(command, 1024, stdin);
    command[strcspn(command, "\n")] = '\0';

    if (strcmp(command, "exit") == 0)
      break;

    num_commands = 0;

    char *cmd_tokens[MAX_COMMANDS];
    char *cmd_saveptr = NULL;
    char *cmd = strtok_r(command, "|", &cmd_saveptr);

    while (cmd != NULL && num_commands < MAX_COMMANDS) {
      while (*cmd == ' ')
        cmd++;
      int len = strlen(cmd);
      while (len > 0 && cmd[len - 1] == ' ') {
        cmd[len - 1] = '\0';
        len--;
      }

      cmd_tokens[num_commands++] = cmd;
      cmd = strtok_r(NULL, "|", &cmd_saveptr);
    }

    for (int i = 0; i < num_commands; i++) {
      int arg_count = 0;
      char *arg_saveptr = NULL;
      char *arg = strtok_r(cmd_tokens[i], " ", &arg_saveptr);

      while (arg != NULL && arg_count < MAX_ARGS) {
        commands[i][arg_count++] = arg;
        arg = strtok_r(NULL, " ", &arg_saveptr);
      }
      commands[i][arg_count] = NULL;
    }

    if (num_commands == 0)
      continue;

    int prev_pipe[2] = {-1, -1};
    int next_pipe[2];

    for (int i = 0; i < num_commands; i++) {
      if (i < num_commands - 1) {
        if (pipe(next_pipe) == -1) {
          perror("pipe");
          exit(EXIT_FAILURE);
        }
      }

      execute_command(commands[i], (i == 0) ? STDIN_FILENO : prev_pipe[0],
                      (i == num_commands - 1) ? STDOUT_FILENO : next_pipe[1]);

      if (prev_pipe[0] != -1)
        close(prev_pipe[0]);
      if (prev_pipe[1] != -1)
        close(prev_pipe[1]);
      if (i < num_commands - 1) {
        close(next_pipe[1]); // Закрываем write-энд в родителе
        prev_pipe[0] = next_pipe[0];
        prev_pipe[1] = -1; // write-энд уже закрыт
      } else {
        prev_pipe[0] = prev_pipe[1] = -1;
      }
    }
    if (prev_pipe[0] != -1) {
      close(prev_pipe[0]);
    }

    for (int i = 0; i < num_commands; i++) {
      wait(NULL);
    }
  }

  return EXIT_SUCCESS;
}