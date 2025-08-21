#ifndef CHAT_H
#define CHAT_H

#include <sys/types.h>

#define MAX_NAME 20
#define MAX_TEXT 256
#define HISTORY_SIZE 100
#define QUEUE_NAME "/chat_service_queue"
#define SHM_NAME "/chat_shared_memory"
#define USERS_MAX 10

enum msg_type {
  NEW_USER = 0,
  NEW_MESSAGE,
  USER_LEFT,
};

typedef struct {
  enum msg_type type;
  char username[MAX_NAME];
  char text[MAX_TEXT];
  pid_t pid;
} message_t;

typedef struct {
  message_t history[HISTORY_SIZE];
  char users[USERS_MAX][MAX_NAME];
  int users_count;
  int current_pos;
  int history_count;
} shared_data_t;

#endif