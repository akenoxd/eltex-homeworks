#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 5
#define INCREMENT_PER_THREAD 100000

int shared_counter = 0;
pthread_spinlock_t spinlock;

void *increment_counter(void *arg) {
  for (int i = 0; i < INCREMENT_PER_THREAD; i++) {
    pthread_spin_lock(&spinlock);
    shared_counter++;
    pthread_spin_unlock(&spinlock);
  }

  pthread_exit(NULL);
}

int main() {
  pthread_t threads[NUM_THREADS];

  if (pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE) != 0) {
    printf("Error initializing spinlock\n");
    exit(EXIT_FAILURE);
  }

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
  printf("    real: %d\n (spinlock)\n", shared_counter);

  pthread_spin_destroy(&spinlock);

  return 0;
}