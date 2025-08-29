#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(9999);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  char buf[100] = "hello";
  if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
    perror("sendto");
    exit(EXIT_FAILURE);
  }

  socklen_t server_len = sizeof(server_addr);
  if (recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&server_addr,
               &server_len) < 0) {
    perror("recvfrom");
    exit(EXIT_FAILURE);
  }

  printf("Current time: %s\n", buf);

  close(sockfd);
  return 0;
}