#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define QUEUE_NAME "/mq_ipcs"
#define MAX_SIZE 256

int main() {
  mqd_t mq;
  char buffer[MAX_SIZE];
  struct mq_attr attr;

  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 512;
  attr.mq_curmsgs = 0;

  mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
  if (mq == (mqd_t)-1) {
    perror("Server: mq_open");
    exit(1);
  }

  sprintf(buffer, "Hi!");
  if (mq_send(mq, buffer, strlen(buffer) + 1, 0) == -1) {
    perror("Server: mq_send");
    exit(1);
  }

  mq_close(mq);
  sleep(1);
  mq = mq_open(QUEUE_NAME, O_RDONLY);

  if (mq_getattr(mq, &attr) == -1) {
    perror("Server: mq_getattr");
    exit(1);
  }
  if (mq_receive(mq, buffer, attr.mq_msgsize, NULL) == -1) {
    perror("Server: mq_receive");
    exit(1);
  }
  printf("Server received: %s\n", buffer);

  // Delete the message queue
  if (mq_close(mq) == -1) {
    perror("mq_close");
    exit(1);
  }
  if (mq_unlink(QUEUE_NAME) == -1) {
    perror("mq_unlink");
    exit(1);
  }

  return 0;
}