#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
int main() {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(9999)};
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
  connect(s, (struct sockaddr *)&addr, sizeof(addr));
  send(s, "hello!", 7, 0);
  char buf[100];
  recv(s, buf, sizeof(buf), 0);
  printf("%s\n", buf);
  close(s);
}