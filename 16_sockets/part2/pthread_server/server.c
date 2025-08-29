#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

void *handle_client(void *arg) {
  int client_fd = *(int *)arg;

  time_t current_time;
  time(&current_time);

  struct tm *tm = localtime(&current_time);
  char time_str[100];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

  if (send(client_fd, time_str, strlen(time_str), 0) < 0) {
    perror("send");
    exit(EXIT_FAILURE);
  }

  close(client_fd);
  return NULL;
}

int main() {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_port = htons(9999),
                             .sin_addr.s_addr = INADDR_ANY};
  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(s, 1) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  while (1) {
    int client_fd = accept(s, NULL, NULL);
    if (client_fd < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    int *client_fd_ptr = malloc(sizeof(int));
    *client_fd_ptr = client_fd;

    pthread_t thread;
    if (pthread_create(&thread, NULL, handle_client, client_fd_ptr) < 0) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }

    pthread_join(thread, NULL);
    free(client_fd_ptr);
  }

  close(s);
  return 0;
}