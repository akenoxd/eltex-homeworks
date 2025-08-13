#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define QUEUE_NAME "/chat_messages"
#define MAX_SIZE 256
#define HISTORY_SIZE 100

enum msg_type {
  NEW_USER = 0,
  NEW_MESSAGE,
  USER_LEFT,
};

typedef struct user_t {
  char name[20];
  mqd_t mq;
  pid_t pid;
} user_t;

int running = 1;

char history[HISTORY_SIZE][MAX_SIZE];
int history_count = 0;

int find_user(user_t *users, int count, pid_t pid) {
  for (int i = 0; i < count; ++i)
    if (users[i].pid == pid)
      return i;
  return -1;
}

void add_to_history(const char *message) {
  if (!message || history_count >= HISTORY_SIZE)
    return;

  strncpy(history[history_count], message, MAX_SIZE - 1);
  history[history_count][MAX_SIZE - 1] = '\0';
  history_count++;
}

void cleanup_resources(mqd_t service_mq, user_t *users, int active_users) {
  char shutdown_msg[MAX_SIZE];
  snprintf(shutdown_msg, sizeof(shutdown_msg),
           "%d\nServer\n0\nServer is shutting down", NEW_MESSAGE);

  for (int i = 0; i < active_users; ++i) {
    mq_send(users[i].mq, shutdown_msg, MAX_SIZE, 0);
    mq_close(users[i].mq);
    char mq_name[64];
    sprintf(mq_name, "/chat_user_%d", users[i].pid);
    mq_unlink(mq_name);
  }

  mq_close(service_mq);
  mq_unlink(QUEUE_NAME);
}

void *input_thread(void *arg) {
  mqd_t service_mq = *(mqd_t *)arg;
  char command[32];
  while (running) {
    if (fgets(command, sizeof(command), stdin)) {
      if (strncmp(command, "/exit", 5) == 0) {
        printf("Shutting down server...\n");
        running = 0;

        // unblock mq_receive
        char dummy_msg[MAX_SIZE];
        snprintf(dummy_msg, sizeof(dummy_msg), "%d\nwe die\n0\n", NEW_MESSAGE);
        mq_send(service_mq, dummy_msg, MAX_SIZE, 0);
        break;
      }
    }
  }
  return NULL;
}

int main() {
  mqd_t service_mq;
  user_t users[10];
  int active_users = 0;
  char buffer[MAX_SIZE];

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 512;
  attr.mq_curmsgs = 0;

  service_mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
  if (service_mq == (mqd_t)-1) {
    perror("Server: mq_open");
    exit(1);
  }

  pthread_t input_tid;
  pthread_create(&input_tid, NULL, input_thread, &service_mq);

  while (running) {
    if (mq_receive(service_mq, buffer, attr.mq_msgsize, NULL) == -1) {
      if (!running)
        break;
      perror("Server: mq_receive");
      break;
    }
    if (!running)
      break;

    char *saveptr;
    char *type_str = strtok_r(buffer, "\n", &saveptr);
    int type = atoi(type_str);

    switch (type) {
    case NEW_USER: {
      char *name = strtok_r(NULL, "\n", &saveptr);
      char *pid_str = strtok_r(NULL, "\n", &saveptr);
      pid_t pid = atoi(pid_str);

      if (active_users < 10) {
        char mq_name[64];
        sprintf(mq_name, "/chat_user_%d", pid);
        mqd_t user_mq = mq_open(mq_name, O_CREAT | O_RDWR, 0644, &attr);

        users[active_users].mq = user_mq;
        users[active_users].pid = pid;
        strncpy(users[active_users].name, name,
                sizeof(users[active_users].name) - 1);
        users[active_users].name[sizeof(users[active_users].name) - 1] = '\0';
        active_users++;

        for (int i = 0; i < history_count; ++i) {
          mq_send(user_mq, history[i], MAX_SIZE, 0);
        }

        char notify[MAX_SIZE];
        snprintf(notify, sizeof(notify), "%d\n%s\n%d\n", NEW_USER, name, pid);
        add_to_history(notify);

        for (int i = 0; i < active_users; ++i)
          mq_send(users[i].mq, notify, MAX_SIZE, 0);

        printf("New user: %s\n", name);
        break;
      }
      break;
    }

    case NEW_MESSAGE: {
      char *name = strtok_r(NULL, "\n", &saveptr);
      char *pid_str = strtok_r(NULL, "\n", &saveptr);
      char *text = strtok_r(NULL, "\n", &saveptr);

      char message[MAX_SIZE];
      snprintf(message, sizeof(message), "%d\n%s\n%d\n%s", NEW_MESSAGE, name,
               atoi(pid_str), text);

      add_to_history(message);

      for (int i = 0; i < active_users; ++i)
        mq_send(users[i].mq, message, MAX_SIZE, 0);

      printf("New message: %s: %s\n", name, text);
      break;
    }

    case USER_LEFT: {
      char *name = strtok_r(NULL, "\n", &saveptr);
      char *pid_str = strtok_r(NULL, "\n", &saveptr);
      pid_t pid = atoi(pid_str);

      int idx = find_user(users, active_users, pid);
      if (idx != -1) {
        mq_close(users[idx].mq);
        char mq_name[64];
        sprintf(mq_name, "/chat_user_%d", pid);
        mq_unlink(mq_name);

        for (int j = idx; j < active_users - 1; ++j)
          users[j] = users[j + 1];
        active_users--;

        char notify[MAX_SIZE];
        snprintf(notify, sizeof(notify), "%d\n%s\n%d\n", USER_LEFT, name, pid);
        add_to_history(notify);

        for (int i = 0; i < active_users; ++i)
          mq_send(users[i].mq, notify, MAX_SIZE, 0);

        printf("User left: %s\n", name);
      }
      break;
    }
    }
  }

  pthread_join(input_tid, NULL);
  cleanup_resources(service_mq, users, active_users);
  printf("Server stopped.\n");
  return 0;
}