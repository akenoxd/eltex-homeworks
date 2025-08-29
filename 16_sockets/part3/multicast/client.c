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
  printf("socket created\n");

  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_port = htons(7777),
                             .sin_addr.s_addr = INADDR_ANY};

  int reuse = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  struct ip_mreq mreq = {
      .imr_multiaddr.s_addr = inet_addr("224.1.1.1"),
      .imr_interface.s_addr = INADDR_ANY,
  };

  if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) == -1) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  printf("waiting for multicast...\n");
  char buf[100];
  for (int i = 0; i < 5; i++) {
    if (recv(s, buf, sizeof(buf), 0) == -1) {
      perror("recv");
      exit(EXIT_FAILURE);
    }
    printf("Received message: %s\n", buf);
  }

  close(s);
  return 0;
}