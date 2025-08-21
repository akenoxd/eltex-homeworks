#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <PID>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  pid_t pid = (pid_t)atoi(argv[1]);

  printf("sending SIGUSR1 to PID %d\n", pid);

  if (kill(pid, SIGUSR1) == -1) {
    perror("kill");
    exit(EXIT_FAILURE);
  }

  printf("signal sent\n");

  return 0;
}