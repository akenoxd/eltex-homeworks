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

  int one = 1;
  if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) == -1) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(9999)};
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

  char packet[1024] = {0};

  struct iphdr *iph = (struct iphdr *)packet;
  iph->version = 4; // IPv4
  iph->ihl = 5; // 5 * 4 = 20 bytes (стандартный размер IP заголовка)
  iph->tos = 0;      // Type of Service
  iph->tot_len = 0;  // Ядро заполнит автоматически
  iph->id = 0;       // Ядро заполнит автоматически
  iph->frag_off = 0; // No fragmentation
  iph->ttl = 64;     // Time to Live
  iph->protocol = IPPROTO_UDP; // UDP protocol
  iph->check = 0; // Ядро рассчитает checksum автоматически
  iph->saddr = 0; // Ядро заполнит автоматически
  inet_pton(AF_INET, "127.0.0.1", &iph->daddr); // destination address

  struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct iphdr));
  char *payload = (char *)(udph + 1);

  char *message = "hello!";
  int data_len = strlen(message) + 1;

  udph->source = htons(7777);
  udph->dest = htons(9999);
  udph->len = htons(sizeof(struct udphdr) + data_len);
  udph->check = 0;

  memcpy(payload, message, data_len);

  int packet_len = sizeof(struct iphdr) + sizeof(struct udphdr) + data_len;

  if (sendto(s, packet, packet_len, 0, (struct sockaddr *)&addr,
             sizeof(addr)) == -1) {
    perror("sendto");
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

    if (iph->protocol != IPPROTO_UDP)
      continue;
    if (ntohs(udph->dest) != 7777)
      continue;
    if (ntohs(udph->source) != 9999)
      continue;

    printf("payload: %s\n", buf + iph->ihl * 4 + sizeof(struct udphdr));
    break;
  }

  close(s);
}