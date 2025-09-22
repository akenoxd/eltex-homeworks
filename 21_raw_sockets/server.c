#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_CLIENTS 100
#define CLOSE_MSG "quit"

// Структура для хранения информации о клиенте
typedef struct {
  struct sockaddr_in addr;
  int counter;
  int used;
} client_info;

client_info clients[MAX_CLIENTS];

// Сравнение клиентов по ip:port
int client_equal(struct sockaddr_in *a, struct sockaddr_in *b) {
  return a->sin_addr.s_addr == b->sin_addr.s_addr && a->sin_port == b->sin_port;
}

// Поиск клиента, если нет — добавление нового
int get_client_index(struct sockaddr_in *addr) {
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].used && client_equal(&clients[i].addr, addr)) {
      return i;
    }
  }
  // Новый клиент
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (!clients[i].used) {
      clients[i].addr = *addr;
      clients[i].counter = 0;
      clients[i].used = 1;
      return i;
    }
  }
  return -1;  // Нет места
}

// Сброс клиента
void reset_client(struct sockaddr_in *addr) {
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].used && client_equal(&clients[i].addr, addr)) {
      clients[i].used = 0;
      break;
    }
  }
}

int main() {
  int s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (s == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  char *packet_in[1024] = {0};
  in_addr_t server_ip;
  // inet_pton(AF_INET, "10.10.10.3", &client_ip);
  inet_pton(AF_INET, "10.10.10.2", &server_ip);

  while (1) {
    int bytes_received = recv(s, packet_in, 1024, 0);
    if (bytes_received == -1) {
      perror("recvfrom");
      break;
    }

    struct ether_header *eth = (struct ether_header *)packet_in;
    // if (memcmp(eth->ether_dhost, client_mac, 6) != 0) continue;
    // if (memcmp(eth->ether_shost, server_mac, 6) != 0) continue;
    if (ntohs(eth->ether_type) != ETHERTYPE_IP) continue;

    struct iphdr *iph =
        (struct iphdr *)(packet_in + sizeof(struct ether_header));
    if (iph->protocol != IPPROTO_UDP) continue;
    // if (iph->daddr != client_ip) continue;
    if (iph->daddr != server_ip) continue;

    struct udphdr *udph = (struct udphdr *)((packet_in + iph->ihl * 4) +
                                            sizeof(struct ether_header));
    if (ntohs(udph->dest) != 7777) continue;
    if (ntohs(udph->source) != 9999) continue;

    char *payload = (char *)(udph + 1);
    uint16_t udp_len = ntohs(udph->len);
    uint16_t payload_len = udp_len - sizeof(struct udphdr);
    payload[payload_len] = '\0';
    printf("Received: %s\n", payload);
    // Проверка на команду закрытия
    if (strcmp(payload, CLOSE_MSG) == 0) {
      reset_client(&client_addr);
      printf("Client disconnected and counter reset\n");
      continue;
    }

    int idx = get_client_index(&client_addr);
    if (idx == -1) {
      fprintf(stderr, "Too many clients\n");
      continue;
    }
    clients[idx].counter++;

    printf("received: %s\n", buf);
    char reply[REPLY_SIZE];
    snprintf(reply, sizeof(reply), "%s %d", buf, clients[idx].counter);
    if (sendto(s, reply, strlen(reply), 0, (struct sockaddr *)(&client_addr),
               client_len) == -1) {
      perror("send");
      exit(EXIT_FAILURE);
    }
  }
  close(s);
}