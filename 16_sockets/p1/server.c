#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SERVER_PATH "/tmp/local_dgram_socket"
#define CLIENT_PATH "/tmp/local_dgram_client_socket"

int main() {
  int sockfd;
  char buf[1024];

  sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if (sockfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  unlink(SERVER_PATH);

  struct sockaddr_un server_addr = {.sun_family = AF_LOCAL,
                                    .sun_path = {SERVER_PATH}};

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    perror("bind");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_un client_addr;
  socklen_t client_len = sizeof(client_addr);

  ssize_t recv_len = recvfrom(sockfd, buf, sizeof(buf), 0,
                              (struct sockaddr *)&client_addr, &client_len);
  if (recv_len == -1) {
    perror("recvfrom");
    exit(EXIT_FAILURE);
  }

  buf[recv_len] = '\0';
  printf("Received from client: %s\n", buf);

  const char *reply = "hi!";
  if (sendto(sockfd, reply, strlen(reply), 0, (struct sockaddr *)&client_addr,
             client_len) == -1) {
    perror("sendto");
  }

  close(sockfd);
  unlink(SERVER_PATH);
  return 0;
}