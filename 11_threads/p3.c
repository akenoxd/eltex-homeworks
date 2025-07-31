#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 5
#define INCREMENT_PER_THREAD 100000

int shared_counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *increment_counter(void *arg) {
  for (int i = 0; i < INCREMENT_PER_THREAD; i++) {
    pthread_mutex_lock(&mutex);
    shared_counter++;
    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(NULL);
}

int main() {
  pthread_t threads[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    int ret = pthread_create(&threads[i], NULL, increment_counter, NULL);
    if (ret) {
      printf("Error creating thread %d\n", i);
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i], NULL);

  printf("expected: %d\n", NUM_THREADS * INCREMENT_PER_THREAD);
  printf("    real: %d\n (mutex)\n", shared_counter);

  pthread_mutex_destroy(&mutex);

  return 0;
}