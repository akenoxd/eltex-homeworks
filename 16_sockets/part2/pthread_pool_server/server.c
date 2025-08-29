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

#define MAX_THREADS 5
#define MAX_CLIENTS 5

typedef struct {
  pthread_t thread;
  int is_free;
} thread_info_t;
thread_info_t thread_pool[MAX_THREADS];

void *handle_client(void *arg) {
  int client_fd = *(int *)arg;
  free(arg);

  time_t current_time;
  time(&current_time);

  struct tm *tm = localtime(&current_time);
  char time_str[100];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

  if (send(client_fd, time_str, strlen(time_str), 0) < 0) {
    perror("send");
    exit(EXIT_FAILURE);
  }
  sleep(1);

  close(client_fd);

  for (int i = 0; i < MAX_THREADS; i++) {
    if (pthread_equal(thread_pool[i].thread, pthread_self())) {
      thread_pool[i].is_free = 1;
      printf("Thread %d is now free\n", i);
      break;
    }
  }

  return NULL;
}

int main() {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  int f = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &f, sizeof(int));

  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_port = htons(9999),
                             .sin_addr.s_addr = INADDR_ANY};
  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(s, MAX_CLIENTS) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < MAX_THREADS; i++)
    thread_pool[i].is_free = 1;

  while (1) {
    int client_fd = accept(s, NULL, NULL);
    if (client_fd < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    printf("Accepted connection, fd: %d\n", client_fd);

    int i = 0;
    while (thread_pool[i].is_free == 0) {
      i++;
      if (i == MAX_THREADS) {
        i = 0;
      }
    }

    int *client_fd_ptr = malloc(sizeof(int));
    *client_fd_ptr = client_fd;
    printf("Assigning to thread %d\n", i);
    pthread_create(&thread_pool[i].thread, NULL, handle_client, client_fd_ptr);
    thread_pool[i].is_free = 0;
  }

  close(s);
  return 0;
}