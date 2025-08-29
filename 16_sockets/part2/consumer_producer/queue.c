#include "queue.h"

void init_queue(thread_queue_t *queue, int capacity) {
  queue->front = NULL;
  queue->rear = NULL;
  queue->size = 0;
  queue->capacity = capacity;
  pthread_mutex_init(&queue->mutex, NULL);
  sem_init(&queue->semaphore, 0, 0);
}

int enqueue(thread_queue_t *queue, int client_fd) {
  pthread_mutex_lock(&queue->mutex);

  if (queue->size >= queue->capacity) {
    pthread_mutex_unlock(&queue->mutex);
    return -1;
  }

  queue_node_t *new_node = (queue_node_t *)malloc(sizeof(queue_node_t));
  if (!new_node) {
    pthread_mutex_unlock(&queue->mutex);
    return -1;
  }

  new_node->client_fd = client_fd;
  new_node->next = NULL;

  if (queue->rear == NULL) {
    queue->front = queue->rear = new_node;
  } else {
    queue->rear->next = new_node;
    queue->rear = new_node;
  }

  queue->size++;
  pthread_mutex_unlock(&queue->mutex);
  sem_post(&queue->semaphore);

  return 0;
}

int dequeue(thread_queue_t *queue, int *client_fd) {
  sem_wait(&queue->semaphore);
  pthread_mutex_lock(&queue->mutex);

  if (queue->front == NULL) {
    pthread_mutex_unlock(&queue->mutex);
    return -1;
  }

  queue_node_t *temp = queue->front;
  *client_fd = temp->client_fd;
  queue->front = queue->front->next;

  if (queue->front == NULL) {
    queue->rear = NULL;
  }

  queue->size--;
  pthread_mutex_unlock(&queue->mutex);

  free(temp);
  return 0;
}

void cleanup_queue(thread_queue_t *queue) {
  pthread_mutex_lock(&queue->mutex);

  while (queue->front != NULL) {
    queue_node_t *temp = queue->front;
    queue->front = queue->front->next;
    close(temp->client_fd);
    free(temp);
  }

  queue->rear = NULL;
  queue->size = 0;
  pthread_mutex_unlock(&queue->mutex);

  sem_destroy(&queue->semaphore);
  pthread_mutex_destroy(&queue->mutex);
}
