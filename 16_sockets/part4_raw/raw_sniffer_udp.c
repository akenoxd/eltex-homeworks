#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {
  int s = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (s == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  while (1) {
    char buf[1024];

    int bytes_received = recv(s, buf, sizeof(buf), 0);
    if (bytes_received == -1) {
      perror("recvfrom");
      exit(EXIT_FAILURE);
    }

    struct iphdr *iph = (struct iphdr *)buf;
    struct udphdr *udph = (struct udphdr *)(buf + iph->ihl * 4);

    char char_addr[INET_ADDRSTRLEN];
    printf("IP Header:\n");
    printf("  Source IP: %s\n",
           inet_ntop(AF_INET, &iph->saddr, char_addr, sizeof(char_addr)));
    printf("  Destination IP: %s\n",
           inet_ntop(AF_INET, &iph->daddr, char_addr, sizeof(char_addr)));
    printf("UDP Header:\n");
    printf("  Source Port: %d\n", ntohs(udph->source));
    printf("  Destination Port: %d\n", ntohs(udph->dest));
    printf("  Length: %d\n", ntohs(udph->len));
    printf("  Checksum: 0x%x\n", ntohs(udph->check));
  }

  close(s);
  return 0;
}