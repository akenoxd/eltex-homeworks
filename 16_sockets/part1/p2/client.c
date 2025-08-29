#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SERVER_PATH "/tmp/stream_sock"

int main() {
  int sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (sock_fd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_un addr = {.sun_family = AF_LOCAL, .sun_path = {SERVER_PATH}};

  if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  if (send(sock_fd, "hello!", 7, 0) == -1) {
    perror("send");
    exit(EXIT_FAILURE);
  }
  char buf[100];

  if (recv(sock_fd, buf, sizeof(buf), 0) == -1) {
    perror("recv");
    exit(EXIT_FAILURE);
  }

  printf("Client received: %s\n", buf);
  close(sock_fd);
}
