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
  setsockopt(s, SOL_SOCKET, IP_MULTICAST_TTL, &f, sizeof(s));

  struct sockaddr_in endpoint = {
      .sin_family = AF_INET, .sin_port = htons(7777), .sin_addr.s_addr = 0};

  if (inet_pton(AF_INET, "224.1.1.1", &endpoint.sin_addr) != 1) {
    exit(EXIT_FAILURE);
  }

  socklen_t client_len = sizeof(endpoint);

  for (int i = 0; i < 5; i++) {
    if (sendto(s, "multicast hiiii!", 17, 0, (struct sockaddr *)(&endpoint),
               client_len) == -1) {
      perror("send");
      exit(EXIT_FAILURE);
    }
    printf("sent %d\n", i);
    sleep(1);
  }

  close(s);
}