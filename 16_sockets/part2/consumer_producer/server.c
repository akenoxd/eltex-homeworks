#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "queue.h"

#define MAX_THREADS 5
#define MAX_CLIENTS 10
#define QUEUE_SIZE 20

typedef struct {
  pthread_t thread;
  int thread_id;
  int is_running;
} thread_info_t;

thread_info_t thread_pool[MAX_THREADS];
thread_queue_t request_queue;

int server_fd;

void handle_client(int client_fd);
void *worker_thread(void *arg);
void signal_handler(int sig);

int main() {
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  init_queue(&request_queue, QUEUE_SIZE);

  for (int i = 0; i < MAX_THREADS; i++) {
    thread_pool[i].is_running = 1;
    thread_pool[i].thread_id = i;

    int *thread_id = malloc(sizeof(int));
    *thread_id = i;

    if (pthread_create(&thread_pool[i].thread, NULL, worker_thread,
                       thread_id) != 0) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }
  }

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  int reuse = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_port = htons(9999),
                             .sin_addr.s_addr = INADDR_ANY};
  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, MAX_CLIENTS) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  while (1) {
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
      perror("accept");
      continue;
    }

    printf("Accepted connection, fd: %d\n", client_fd);

    if (enqueue(&request_queue, client_fd) < 0) {
      printf("Queue is full, rejecting client fd: %d\n", client_fd);
      close(client_fd);
    } else {
      printf("Client fd: %d added to queue: %d/%d\n", client_fd,
             request_queue.size, request_queue.capacity);
    }
  }

  return 0;
}

void handle_client(int client_fd) {
  time_t current_time;
  time(&current_time);

  struct tm *tm = localtime(&current_time);
  char time_str[100];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

  if (send(client_fd, time_str, strlen(time_str), 0) < 0) {
    perror("send");
  }

  sleep(2);
  close(client_fd);
}

void *worker_thread(void *arg) {
  int thread_id = *(int *)arg;
  free(arg);

  while (thread_pool[thread_id].is_running) {
    int client_fd;

    if (dequeue(&request_queue, &client_fd) == 0) {
      printf("Thread %d processing client fd: %d\n", thread_id, client_fd);
      handle_client(client_fd);
      printf("Thread %d is free\n", thread_id);
    }
  }

  printf("Worker thread %d stopped\n", thread_id);
  return NULL;
}

void signal_handler(int sig) {
  printf("\nReceiving signal %d. Shutting down server...\n", sig);

  for (int i = 0; i < MAX_THREADS; i++)
    thread_pool[i].is_running = 0;

  for (int i = 0; i < MAX_THREADS; i++)
    sem_post(&request_queue.semaphore);

  cleanup_queue(&request_queue);
  close(server_fd);

  exit(0);
}