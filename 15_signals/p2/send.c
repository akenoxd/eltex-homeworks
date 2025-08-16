#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Использование: %s <PID приемника>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  pid_t pid = (pid_t)atoi(argv[1]);

  printf("Отправка SIGUSR1 процессу с PID %d\n", pid);

  if (kill(pid, SIGUSR1) == -1) {
    perror("Ошибка kill");
    exit(EXIT_FAILURE);
  }

  printf("Сигнал успешно отправлен\n");

  return 0;
}