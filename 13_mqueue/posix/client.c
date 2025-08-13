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

  mq = mq_open(QUEUE_NAME, O_RDWR);
  if (mq == (mqd_t)-1) {
    perror("Client: mq_open");
    exit(1);
  }

  struct mq_attr attr;
  if (mq_getattr(mq, &attr) == -1) {
    perror("Client: mq_getattr");
    exit(1);
  }

  if (mq_receive(mq, buffer, attr.mq_msgsize, NULL) == -1) {
    perror("Client: mq_receive");
    exit(1);
  }

  printf("Client received: %s\n", buffer);

  mq_close(mq);

  mq = mq_open(QUEUE_NAME, O_WRONLY);
  // if (mq == (mqd_t)-1) {
  //   perror("Client: mq_open 2");
  //   exit(1);
  // }
  if (mq_send(mq, "Hello!", strlen("Hello!") + 1, 0) == -1) {
    perror("Client: mq_send");
    exit(1);
  }
  // Close the message queue
  mq_close(mq);
  sleep(1);
  return 0;
}