#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROOMS_COUNT 5
#define DELAY_CONSUMER 1
#define DELAY_PRODUCER 2

int shop_rooms[ROOMS_COUNT];
int consumer_need = 100;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *consumer(void *arg) {
  while (1) {
    pthread_mutex_lock(&mutex);

    if (consumer_need <= 0) {
      pthread_mutex_unlock(&mutex);
      break;
    }

    int room = rand() % ROOMS_COUNT;
    int goods = shop_rooms[room];

    if (goods > 0) {
      int taken = (goods > consumer_need) ? consumer_need : goods;
      shop_rooms[room] -= taken;
      consumer_need -= taken;

      printf("Consumer took %d from room %d (need left: %d)\n", taken, room,
             consumer_need);
    } else {
      printf("Consumer found room %d empty\n", room);
    }

    pthread_mutex_unlock(&mutex);
    sleep(DELAY_CONSUMER);
  }

  printf("Consumer satisfied all needs!\n");
  return NULL;
}

void *producer(void *arg) {
  while (1) {
    pthread_mutex_lock(&mutex);

    if (consumer_need <= 0) {
      pthread_mutex_unlock(&mutex);
      break;
    }

    int room = rand() % ROOMS_COUNT;
    int add = 10 + rand() % 30; // Добавляем от 10 до 30 товаров
    shop_rooms[room] += add;

    printf("Producer added %d to room %d (total there: %d)\n", add, room,
           shop_rooms[room]);

    pthread_mutex_unlock(&mutex);
    sleep(DELAY_PRODUCER);
  }

  printf("Producer finished work\n");
  return NULL;
}

int main() {
  srand(time(NULL));

  for (int i = 0; i < ROOMS_COUNT; i++) {
    shop_rooms[i] = 10 + rand() % 30;
    printf("Room %d: %d goods\n", i, shop_rooms[i]);
  }
  printf("Consumer need: %d\n\n", consumer_need);

  pthread_t consumer_thread, producer_thread;

  pthread_create(&consumer_thread, NULL, consumer, NULL);
  pthread_create(&producer_thread, NULL, producer, NULL);

  pthread_join(consumer_thread, NULL);
  pthread_join(producer_thread, NULL);

  printf("\nFinal state:\n");
  for (int i = 0; i < ROOMS_COUNT; i++) {
    printf("Room %d: %d goods\n", i, shop_rooms[i]);
  }

  pthread_mutex_destroy(&mutex);
  return 0;
}
