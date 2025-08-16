#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// Обработчик сигнала
void handler(int sig, siginfo_t* siginfo, void* arg) {
  if (sig == SIGUSR1) {
    printf("Получен сигнал SIGUSR1\n");
  }
}

int main() {
  struct sigaction act = {0};

  act.sa_flags = 0;
  act.sa_sigaction = &handler;
  if (sigaction(SIGUSR1, &act, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  printf("Started with pid: %d\n", getpid());

  // Бесконечный цикл ожидания
  while (1) {
    pause();
  }

  return 0;
}