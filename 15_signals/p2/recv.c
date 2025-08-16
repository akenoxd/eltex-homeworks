#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  sigset_t mask;

  // Инициализируем набор сигналов
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);  // Добавляем SIGINT в набор

  // Блокируем SIGINT
  if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
    perror("Ошибка sigprocmask");
    exit(EXIT_FAILURE);
  }

  printf("Сигнал SIGINT (Ctrl+C) теперь заблокирован\n");
  printf("Программа находится в бесконечном цикле. PID: %d\n", getpid());
  printf("Для завершения можно использовать: kill -9 %d\n", getpid());

  // Бесконечный цикл
  while (1) {
    sleep(1);  // Чтобы не нагружать процессор
  }

  return 0;
}