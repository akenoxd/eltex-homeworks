#include <arpa/inet.h>
#include <errno.h>
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
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_CLIENTS 100
#define BUF_SIZE 1024
#define REPLY_SIZE 256
#define CLOSE_MSG "quit"

volatile sig_atomic_t stop_flag = 0;

// Структура для хранения информации о клиенте
typedef struct {
  struct in_addr ip;
  uint16_t port;
  int counter;
  int used;
} client_info;

client_info clients[MAX_CLIENTS];

// Сравнение клиентов по ip:port
int client_equal(struct in_addr *ip1, uint16_t port1, struct in_addr *ip2,
                 uint16_t port2) {
  return ip1->s_addr == ip2->s_addr && port1 == port2;
}

// Поиск клиента, если нет — добавление нового
int get_client_index(struct in_addr *ip, uint16_t port) {
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].used &&
        client_equal(&clients[i].ip, clients[i].port, ip, port)) {
      return i;
    }
  }
  // Новый клиент
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (!clients[i].used) {
      clients[i].ip = *ip;
      clients[i].port = port;
      clients[i].counter = 0;
      clients[i].used = 1;
      return i;
    }
  }
  return -1;  // Нет места
}

// Сброс клиента
void reset_client(struct in_addr *ip, uint16_t port) {
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].used &&
        client_equal(&clients[i].ip, clients[i].port, ip, port)) {
      clients[i].used = 0;
      printf("Client %s:%d disconnected and counter reset\n", inet_ntoa(*ip),
             port);
      break;
    }
  }
}

uint16_t ip_checksum(void *header) {
  uint32_t sum = 0;
  uint16_t *ptr = (uint16_t *)header;

  for (int i = 0; i < 10; i++) {
    sum += ntohs(ptr[i]);
  }

  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }

  return htons((uint16_t)~sum);
}

void create_reply_packet(char *packet, struct ether_header *received_eth,
                         struct iphdr *received_iph,
                         struct udphdr *received_udph, char *payload,
                         int payload_len, int client_counter) {
  // Ethernet header
  struct ether_header *eth = (struct ether_header *)packet;

  // Swap MAC addresses using temporary variable
  unsigned char temp_mac[6];
  memcpy(temp_mac, received_eth->ether_dhost, 6);
  memcpy(eth->ether_dhost, received_eth->ether_shost,
         6);  // Destination = original source

  memcpy(eth->ether_shost, received_eth->ether_dhost,
         6);  // Source = original destination

  eth->ether_type = htons(ETHERTYPE_IP);

  // IP header
  struct iphdr *iph = (struct iphdr *)(packet + sizeof(struct ether_header));
  iph->version = 4;
  iph->ihl = 5;
  iph->tos = 0;

  // Calculate total length correctly
  char reply_msg[REPLY_SIZE];
  snprintf(reply_msg, sizeof(reply_msg), "%s %d", payload, client_counter);
  int reply_msg_len = strlen(reply_msg);

  iph->tot_len =
      htons(sizeof(struct iphdr) + sizeof(struct udphdr) + reply_msg_len);
  iph->id = 0;
  iph->frag_off = 0;
  iph->ttl = 64;
  iph->protocol = IPPROTO_UDP;
  iph->check = 0;

  // Swap IP addresses
  iph->saddr = received_iph->daddr;  // Server IP (was destination in request)
  iph->daddr = received_iph->saddr;  // Client IP (was source in request)

  iph->check = ip_checksum(iph);

  // UDP header
  struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct ether_header) +
                                          sizeof(struct iphdr));
  udph->source =
      received_udph->dest;  // Server port (was destination in request)
  udph->dest = received_udph->source;  // Client port (was source in request)
  udph->len = htons(sizeof(struct udphdr) + reply_msg_len);
  udph->check = 0;  // UDP checksum is optional for IPv4

  // Payload
  char *reply_payload = (char *)(udph + 1);
  memcpy(reply_payload, reply_msg, reply_msg_len);
}

int main() {
  int s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (s == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Bind to interface
  struct sockaddr_ll sll = {0};
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = if_nametoindex("eth0");
  sll.sll_protocol = htons(ETH_P_ALL);

  if (bind(s, (struct sockaddr *)&sll, sizeof(sll)) == -1) {
    perror("bind");
    close(s);
    exit(EXIT_FAILURE);
  }

  printf(
      "Raw socket server started on eth0, listening for UDP packets on port "
      "9999...\n");

  char buf[BUF_SIZE];
  struct in_addr server_ip;
  inet_pton(AF_INET, "10.10.10.2", &server_ip);

  while (!stop_flag) {
    int bytes_received = recv(s, buf, sizeof(buf), 0);
    if (bytes_received == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        continue;
      } else {
        perror("recv");
        break;
      }
    }

    // Check if packet is large enough to contain Ethernet + IP headers
    if (bytes_received <
        (int)(sizeof(struct ether_header) + sizeof(struct iphdr))) {
      continue;
    }

    struct ether_header *eth = (struct ether_header *)buf;

    // Only process IP packets
    if (ntohs(eth->ether_type) != ETHERTYPE_IP) {
      continue;
    }

    struct iphdr *iph = (struct iphdr *)(buf + sizeof(struct ether_header));

    // Only process UDP packets
    if (iph->protocol != IPPROTO_UDP) {
      continue;
    }

    // Check if packet is destined to our server
    if (iph->daddr != server_ip.s_addr) {
      continue;
    }

    // Check if packet is large enough to contain UDP header
    if (bytes_received < (int)(sizeof(struct ether_header) + iph->ihl * 4 +
                               sizeof(struct udphdr))) {
      continue;
    }

    struct udphdr *udph =
        (struct udphdr *)(buf + sizeof(struct ether_header) + iph->ihl * 4);

    // Check if packet is for our server port
    if (ntohs(udph->dest) != 9999) {
      continue;
    }

    // Extract payload
    char *payload = (char *)(udph + 1);
    uint16_t udp_len = ntohs(udph->len);
    uint16_t payload_len = udp_len - sizeof(struct udphdr);

    // Ensure we don't read beyond received data
    if (bytes_received <
        (int)(sizeof(struct ether_header) + iph->ihl * 4 + udp_len)) {
      continue;
    }

    // Null-terminate the payload for string processing
    if (payload_len > 0) {
      int safe_payload_len = (payload_len < (BUF_SIZE - (payload - buf) - 1))
                                 ? payload_len
                                 : (BUF_SIZE - (payload - buf) - 1);
      payload[safe_payload_len] = '\0';
    } else {
      continue;  // Skip empty packets
    }

    // Get client info
    struct in_addr client_ip;
    client_ip.s_addr = iph->saddr;
    uint16_t client_port = ntohs(udph->source);

    printf("Received from %s:%d: %s\n", inet_ntoa(client_ip), client_port,
           payload);

    // Check for quit message
    if (strcmp(payload, CLOSE_MSG) == 0) {
      reset_client(&client_ip, client_port);
      continue;
    }

    // Get or create client
    int idx = get_client_index(&client_ip, udph->source);
    if (idx == -1) {
      fprintf(stderr, "Too many clients, ignoring message from %s:%d\n",
              inet_ntoa(client_ip), client_port);
      continue;
    }

    clients[idx].counter++;

    // Create reply packet
    char reply_packet[BUF_SIZE];
    memset(reply_packet, 0, sizeof(reply_packet));
    create_reply_packet(reply_packet, eth, iph, udph, payload, payload_len,
                        clients[idx].counter);

    // Prepare destination address for raw socket
    struct sockaddr_ll dest_addr = {0};
    dest_addr.sll_family = AF_PACKET;
    dest_addr.sll_ifindex = if_nametoindex("eth0");
    dest_addr.sll_halen = ETH_ALEN;
    memcpy(dest_addr.sll_addr, eth->ether_shost, 6);  // Send to client's MAC
    dest_addr.sll_protocol = htons(ETH_P_IP);

    // Calculate packet length correctly
    char reply_msg[REPLY_SIZE];
    snprintf(reply_msg, sizeof(reply_msg), "%s %d", payload,
             clients[idx].counter);
    int packet_len = sizeof(struct ether_header) + sizeof(struct iphdr) +
                     sizeof(struct udphdr) + strlen(reply_msg);

    // Send only ONE reply
    if (sendto(s, reply_packet, packet_len, 0, (struct sockaddr *)&dest_addr,
               sizeof(dest_addr)) == -1) {
      perror("sendto");
    } else {
      printf("Sent reply to %s:%d: %s %d\n", inet_ntoa(client_ip), client_port,
             payload, clients[idx].counter);
    }
  }

  close(s);
  return 0;
}