#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  printf("process0: PID = %d, PPID = %d\n", getpid(), getppid());
  pid_t pid2;
  pid_t pid1 = fork();

  if (pid1 == 0) {
    printf("\tprocess1: PID = %d, PPID = %d\n", getpid(), getppid());
    pid_t pid4;
    pid_t pid3 = fork();
    if (pid3 == 0) {
      printf("\t\tprocess3: PID = %d, PPID = %d\n", getpid(), getppid());
      exit(EXIT_SUCCESS);
    } else {
      pid4 = fork();
      if (pid4 == 0) {
        printf("\t\tprocess4: PID = %d, PPID = %d\n", getpid(), getppid());
        exit(EXIT_SUCCESS);
      }
    }

    waitpid(pid3, 0, 0);
    waitpid(pid4, 0, 0);
    exit(EXIT_SUCCESS);
  } else {
    pid2 = fork();
    if (pid2 == 0) {
      printf("\tprocess2: PID = %d, PPID = %d\n", getpid(), getppid());
      pid_t pid5 = fork();
      if (pid5 == 0) {
        printf("\t\tprocess5: PID = %d, PPID = %d\n", getpid(), getppid());
        exit(EXIT_SUCCESS);
      }
      waitpid(pid5, 0, 0);
      exit(EXIT_SUCCESS);
    }
  }

  waitpid(pid1, 0, 0);
  waitpid(pid2, 0, 0);
  return EXIT_SUCCESS;
}