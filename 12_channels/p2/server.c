#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/my_fifo"

int main() {
  if (mkfifo(FIFO_NAME, 0666) == -1) {
    perror("mkfifo");
    exit(EXIT_FAILURE);
  }

  int fd = open(FIFO_NAME, O_WRONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  const char *message = "Hi!";
  if (write(fd, message, strlen(message) + 1) == -1) {
    perror("write");
    close(fd);
    exit(EXIT_FAILURE);
  }

  close(fd);
  return 0;
}