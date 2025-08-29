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
  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_port = htons(9999),
                             .sin_addr.s_addr = INADDR_ANY};

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  char buf[100];
  while (1) {
    if (recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)(&client_addr),
                 &client_len) == -1) {
      perror("recv");
      exit(EXIT_FAILURE);
    }
    printf("received: %s\n", buf);
    char reply[256];
    sprintf(reply, "%s %s", buf, "reply");
    if (sendto(s, reply, 256, 0, (struct sockaddr *)(&client_addr),
               client_len) == -1) {
      perror("send");
      exit(EXIT_FAILURE);
    }
  }
  close(s);
}