#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {
  int s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  int f = 1;
  setsockopt(s, SOL_SOCKET, SO_BROADCAST, &f, sizeof(s));

  int reuse = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

  struct sockaddr_in addr = {
      .sin_family = AF_INET, .sin_port = htons(7777), .sin_addr.s_addr = 0};

  if (inet_pton(AF_INET, "255.255.255.255", &addr.sin_addr) != 1) {
    exit(EXIT_FAILURE);
  }

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  char buf[100];
  if (recv(s, buf, sizeof(buf), 0) == -1) {
    perror("recv");
    exit(EXIT_FAILURE);
  }
  printf("%s\n", buf);

  close(s);
}