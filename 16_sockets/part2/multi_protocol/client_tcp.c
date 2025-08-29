#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(9999)};
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
  if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  char buf[100];
  if (recv(s, buf, sizeof(buf), 0) < 0) {
    perror("recv");
    exit(EXIT_FAILURE);
  }

  printf("Current time: %s\n", buf);

  close(s);
  return 0;
}