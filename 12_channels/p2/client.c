#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/my_fifo"

int main() {
  sleep(1);
  int fd = open(FIFO_NAME, O_RDONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  char buffer[100];
  if (read(fd, buffer, sizeof(buffer)) == -1) {
    perror("read");
    close(fd);
    exit(EXIT_FAILURE);
  }

  printf("Received message: %s\n", buffer);

  close(fd);
  if (unlink(FIFO_NAME) == -1) {
    perror("unlink");
    exit(EXIT_FAILURE);
  }

  return 0;
}