#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_SIZE 1024
#define S_KEY 42

int main() {
  int shmid;
  char *shm_ptr;

  shmid = shmget(S_KEY, SHM_SIZE, IPC_CREAT | 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  shm_ptr = (char *)shmat(shmid, NULL, 0);
  if (shm_ptr == (char *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  strcpy(shm_ptr, "Hi!");

  while (shm_ptr[0] == 'H' && shm_ptr[1] == 'i' && shm_ptr[2] == '!')
    sleep(1);

  printf("Server received: %s\n", shm_ptr);

  if (shmdt(shm_ptr) == -1) {
    perror("shmdt");
  }

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    exit(EXIT_FAILURE);
  }

  return 0;
}