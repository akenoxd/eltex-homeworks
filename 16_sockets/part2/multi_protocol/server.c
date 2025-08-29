#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

void handle_client(int client_fd) {
  time_t current_time;
  time(&current_time);

  struct tm *tm = localtime(&current_time);
  char time_str[100];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

  if (send(client_fd, time_str, strlen(time_str), 0) < 0) {
    perror("send");
    exit(EXIT_FAILURE);
  }

  close(client_fd);
  return;
}

void handle_udp(int udp_fd) {
  char buf[100];
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);

  if (recvfrom(udp_fd, buf, sizeof(buf), 0, (struct sockaddr *)&addr, &len) <
      0) {
    perror("recvfrom");
    exit(EXIT_FAILURE);
  }

  time_t current_time;
  time(&current_time);

  struct tm *tm = localtime(&current_time);
  char time_str[100];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

  if (sendto(udp_fd, time_str, strlen(time_str), 0, (struct sockaddr *)&addr,
             len) < 0) {
    perror("sendto");
    exit(EXIT_FAILURE);
  }
}

int main() {
  int tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (tcp_fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_port = htons(9999),
                             .sin_addr.s_addr = INADDR_ANY};

  if (bind(tcp_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (bind(udp_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(tcp_fd, 1) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  struct pollfd fds[2];
  fds[0].fd = tcp_fd;
  fds[0].events = POLLIN;
  fds[1].fd = udp_fd;
  fds[1].events = POLLIN;

  while (1) {
    int ret = poll(fds, 2, -1);
    if (ret < 0) {
      perror("poll");
      exit(EXIT_FAILURE);
    }

    if (fds[0].revents & POLLIN) {
      int client_fd = accept(tcp_fd, NULL, NULL);
      if (client_fd < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
      }

      handle_client(client_fd);
    }

    if (fds[1].revents & POLLIN) {
      handle_udp(udp_fd);
    }
  }

  close(tcp_fd);
  close(udp_fd);
  return 0;
}