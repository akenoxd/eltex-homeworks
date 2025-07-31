#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 5

void *thread_function(void *arg) {

  printf("thread %ld\n", pthread_self());
  pthread_exit(NULL);
}

int main() {
  pthread_t threads[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    int ret = pthread_create(&threads[i], NULL, thread_function, NULL);
    if (ret) {
      printf("Error creating thread %d\n", i);
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i], NULL);

  return 0;
}