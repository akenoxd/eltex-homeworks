#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
int main() {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_port = htons(9999),
                             .sin_addr.s_addr = INADDR_ANY};
  bind(s, (struct sockaddr *)&addr, sizeof(addr));
  listen(s, 1);
  int c = accept(s, NULL, NULL);
  char buf[100];
  recv(c, buf, sizeof(buf), 0);
  printf("%s\n", buf);
  send(c, "hi!", 4, 0);
  close(c);
  close(s);
}