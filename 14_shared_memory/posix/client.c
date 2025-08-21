#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHM_SIZE 1024
#define SHM_NAME "/my_shared_memory"

int main() {
  int shm_fd;
  char *shm_ptr;

  shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  shm_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_ptr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  close(shm_fd);

  printf("Client received: %s\n", shm_ptr);

  strcpy(shm_ptr, "Hello!");

  if (munmap(shm_ptr, SHM_SIZE) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }

  return 0;
}