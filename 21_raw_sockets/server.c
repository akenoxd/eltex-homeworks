#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_CLIENTS 100
#define BUF_SIZE 100
#define REPLY_SIZE 256
#define CLOSE_MSG "quit"

// Структура для хранения информации о клиенте
typedef struct {
  struct sockaddr_in addr;
  int counter;
  int used;
} client_info;

client_info clients[MAX_CLIENTS];

// Сравнение клиентов по ip:port
int client_equal(struct sockaddr_in *a, struct sockaddr_in *b) {
  return a->sin_addr.s_addr == b->sin_addr.s_addr && a->sin_port == b->sin_port;
}

// Поиск клиента, если нет — добавление нового
int get_client_index(struct sockaddr_in *addr) {
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].used && client_equal(&clients[i].addr, addr)) {
      return i;
    }
  }
  // Новый клиент
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (!clients[i].used) {
      clients[i].addr = *addr;
      clients[i].counter = 0;
      clients[i].used = 1;
      return i;
    }
  }
  return -1;  // Нет места
}

// Сброс клиента
void reset_client(struct sockaddr_in *addr) {
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].used && client_equal(&clients[i].addr, addr)) {
      clients[i].used = 0;
      break;
    }
  }
}

int main() {
  int s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_port = htons(9999),
                             .sin_addr.s_addr = INADDR_ANY};

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  char buf[BUF_SIZE];
  while (1) {
    ssize_t recv_len = recvfrom(s, buf, sizeof(buf) - 1, 0,
                                (struct sockaddr *)(&client_addr), &client_len);
    if (recv_len == -1) {
      perror("recv");
      exit(EXIT_FAILURE);
    }
    buf[recv_len] = '\0';

    // Проверка на команду закрытия
    if (strcmp(buf, CLOSE_MSG) == 0) {
      reset_client(&client_addr);
      printf("Client disconnected and counter reset\n");
      continue;
    }

    int idx = get_client_index(&client_addr);
    if (idx == -1) {
      fprintf(stderr, "Too many clients\n");
      continue;
    }
    clients[idx].counter++;

    printf("received: %s\n", buf);
    char reply[REPLY_SIZE];
    snprintf(reply, sizeof(reply), "%s %d", buf, clients[idx].counter);
    if (sendto(s, reply, strlen(reply), 0, (struct sockaddr *)(&client_addr),
               client_len) == -1) {
      perror("send");
      exit(EXIT_FAILURE);
    }
  }
  close(s);
}