#include <mqueue.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define QUEUE_NAME "/chat_messages"
#define MAX_SIZE 256
#define USERS_MAX 10

enum msg_type {
  NEW_USER = 0,
  NEW_MESSAGE,
  USER_LEFT,
};

typedef struct {
  mqd_t service_mq;
  char name[10];
  pid_t uid;
} sender_args_t;

typedef struct {
  mqd_t message_mq;
} receiver_args_t;

WINDOW *chat_win, *users_win, *input_win;
int chat_lines = 0;
char chat_history[100][MAX_SIZE];
char users[USERS_MAX][20];
int users_count = 0;
pthread_mutex_t ui_mutex = PTHREAD_MUTEX_INITIALIZER;

void redraw_chat() {
  pthread_mutex_lock(&ui_mutex);
  werase(chat_win);
  box(chat_win, 0, 0);
  int start = chat_lines > LINES - 6 ? chat_lines - (LINES - 6) : 0;
  for (int i = start; i < chat_lines; ++i) {
    mvwaddnstr(chat_win, i - start + 1, 1, chat_history[i], COLS - 22);
  }
  wrefresh(chat_win);
  pthread_mutex_unlock(&ui_mutex);
}

void redraw_users() {
  pthread_mutex_lock(&ui_mutex);
  werase(users_win);
  box(users_win, 0, 0);
  mvwprintw(users_win, 0, 1, "Users:");
  for (int i = 0; i < users_count; ++i) {
    mvwprintw(users_win, i + 1, 1, "%s", users[i]);
  }
  wrefresh(users_win);
  pthread_mutex_unlock(&ui_mutex);
}

void add_user(const char *name) {
  if (!name)
    return;

  if (users_count < USERS_MAX) {
    strncpy(users[users_count], name, sizeof(users[users_count]) - 1);
    users[users_count][sizeof(users[users_count]) - 1] = '\0';
    users_count++;
    redraw_users();
  }
}

void remove_user(const char *name) {
  for (int i = 0; i < users_count; ++i) {
    if (strcmp(users[i], name) == 0) {
      for (int j = i; j < users_count - 1; ++j)
        strcpy(users[j], users[j + 1]);
      users_count--;
      redraw_users();
      break;
    }
  }
}

void *receiver_thread(void *arg) {
  receiver_args_t *args = (receiver_args_t *)arg;
  char buffer[MAX_SIZE];
  struct mq_attr attr;
  mq_getattr(args->message_mq, &attr);

  while (1) {
    ssize_t bytes = mq_receive(args->message_mq, buffer, attr.mq_msgsize, NULL);
    if (bytes > 0) {
      buffer[bytes] = '\0';

      char *saveptr;
      char *type_str = strtok_r(buffer, "\n", &saveptr);
      if (!type_str)
        continue;

      int type = atoi(type_str);
      char *name = strtok_r(NULL, "\n", &saveptr);
      if (!name)
        continue;

      switch (type) {
      case NEW_USER: {
        add_user(name);
        snprintf(chat_history[chat_lines], MAX_SIZE - 1,
                 "User %s joined the chat!", name);
        break;
      }
      case USER_LEFT: {
        remove_user(name);
        snprintf(chat_history[chat_lines], MAX_SIZE - 1,
                 "User %s left the chat!", name);
        break;
      }
      case NEW_MESSAGE: {
        // char *uid_str=
        strtok_r(NULL, "\n", &saveptr);
        char *text = strtok_r(NULL, "\n", &saveptr);
        if (text) {
          snprintf(chat_history[chat_lines], MAX_SIZE - 1, "%s: %s", name,
                   text);
        }
        break;
      }
      }

      chat_history[chat_lines][MAX_SIZE - 1] = '\0';
      chat_lines++;
      if (chat_lines >= 100)
        chat_lines = 99;
      redraw_chat();
    }
  }
  return NULL;
}

void *sender_thread(void *arg) {
  int leaving = 0;
  sender_args_t *args = (sender_args_t *)arg;
  char message[MAX_SIZE];
  char text[MAX_SIZE - 32];

  while (1) {
    pthread_mutex_lock(&ui_mutex);
    werase(input_win);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, "> ");
    wrefresh(input_win);
    pthread_mutex_unlock(&ui_mutex);

    echo();
    wgetnstr(input_win, text, sizeof(text) - 1);
    noecho();

    if (strlen(text) == 0)
      continue;

    if (strcmp(text, "/exit") == 0) {
      snprintf(message, sizeof(message), "%d\n%s\n%d\n", USER_LEFT, args->name,
               args->uid);
      leaving = 1;
    } else {
      snprintf(message, sizeof(message), "%d\n%s\n%d\n%s", NEW_MESSAGE,
               args->name, args->uid, text);
    }
    mq_send(args->service_mq, message, MAX_SIZE, 0);
    if (leaving)
      break;
  }
  return NULL;
}

int main() {
  mqd_t service_mq;

  service_mq = mq_open(QUEUE_NAME, O_WRONLY);
  if (service_mq == (mqd_t)-1) {
    perror("Client: mq_open");
    exit(1);
  }

  char name[10];
  printf("Enter your name: ");
  scanf("%9s", name);
  getchar();

  pid_t uid = getpid();
  char message[MAX_SIZE];
  sprintf(message, "%d\n%s\n%d", NEW_USER, name, uid);
  mq_send(service_mq, message, MAX_SIZE, 0);

  char mq_name[32];
  sprintf(mq_name, "/chat_user_%d", uid);

  sleep(2);
  mqd_t message_mq = mq_open(mq_name, O_RDONLY);
  if (message_mq == (mqd_t)-1) {
    perror("Client: mq_open user queue");
    exit(1);
  }

  initscr();
  cbreak();
  curs_set(1);
  keypad(stdscr, TRUE);

  int chat_height = LINES - 4;
  int users_width = 20;
  chat_win = newwin(chat_height, COLS - users_width, 0, 0);
  users_win = newwin(chat_height, users_width, 0, COLS - users_width);
  input_win = newwin(4, COLS, chat_height, 0);

  scrollok(chat_win, TRUE);
  keypad(input_win, TRUE);

  box(chat_win, 0, 0);
  box(users_win, 0, 0);
  box(input_win, 0, 0);
  mvwprintw(users_win, 0, 1, "Users:");
  mvwprintw(input_win, 1, 1, "> ");

  wrefresh(chat_win);
  wrefresh(users_win);
  wrefresh(input_win);

  pthread_t recv_tid, send_tid;
  receiver_args_t recv_args = {.message_mq = message_mq};
  sender_args_t send_args = {.service_mq = service_mq};
  strncpy(send_args.name, name, sizeof(send_args.name));
  send_args.uid = uid;

  pthread_create(&recv_tid, NULL, receiver_thread, &recv_args);
  pthread_create(&send_tid, NULL, sender_thread, &send_args);

  pthread_join(send_tid, NULL);
  pthread_cancel(recv_tid);

  mq_close(service_mq);
  mq_close(message_mq);

  delwin(chat_win);
  delwin(users_win);
  delwin(input_win);
  endwin();

  return 0;
}