#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_port = htons(9999),
                             .sin_addr.s_addr = INADDR_ANY};
  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
  if (listen(s, 1) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  int c = accept(s, NULL, NULL);
  if (c < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  char buf[100];
  if (recv(c, buf, sizeof(buf), 0) < 0) {
    perror("recv");
    exit(EXIT_FAILURE);
  }

  printf("Server received: %s\n", buf);
  if (send(c, "hi!", 4, 0) < 0) {
    perror("send");
    exit(EXIT_FAILURE);
  }

  close(c);
  close(s);
}