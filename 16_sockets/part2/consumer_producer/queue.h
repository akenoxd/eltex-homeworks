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

typedef struct queue_node {
  int client_fd;
  struct queue_node *next;
} queue_node_t;

typedef struct {
  queue_node_t *front;
  queue_node_t *rear;
  int size;
  int capacity;
  pthread_mutex_t mutex;
  sem_t semaphore;
} thread_queue_t;

void init_queue(thread_queue_t *queue, int capacity);
int enqueue(thread_queue_t *queue, int client_fd);
int dequeue(thread_queue_t *queue, int *client_fd);
void cleanup_queue(thread_queue_t *queue);