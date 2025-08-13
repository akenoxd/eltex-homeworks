#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_SIZE 100

struct msg_buffer {
  long msg_type;
  char msg_text[MSG_SIZE];
};

int main() {
  key_t key = ftok("msgqueue", 42);
  int msgid = msgget(key, 0666 | IPC_CREAT);

  struct msg_buffer message;

  msgrcv(msgid, &message, sizeof(message), 1, 0);
  printf("Client received: %s\n", message.msg_text);

  message.msg_type = 2;
  strcpy(message.msg_text, "Hello!");
  msgsnd(msgid, &message, sizeof(message), 0);

  msgctl(msgid, IPC_RMID, NULL);

  return 0;
}