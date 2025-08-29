#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SERVER_PATH "/tmp/stream_sock"

int main() {
  int server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  unlink(SERVER_PATH);

  struct sockaddr_un addr = {.sun_family = AF_LOCAL, .sun_path = {SERVER_PATH}};

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 1) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  int client_fd = accept(server_fd, NULL, NULL);
  if (client_fd == -1) {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  char buf[100];

  if (recv(client_fd, buf, sizeof(buf), 0) == -1) {
    perror("recv");
    exit(EXIT_FAILURE);
  }

  printf("Server received: %s\n", buf);

  if (send(client_fd, "hi!", 4, 0) == -1) {
    perror("send");
    exit(EXIT_FAILURE);
  }

  close(client_fd);
  close(server_fd);
  unlink(SERVER_PATH);
}