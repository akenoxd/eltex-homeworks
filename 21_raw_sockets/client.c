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

#define MAX_INPUT_SIZE 256

volatile sig_atomic_t stop_flag = 0;

// void handle_sigint(int sig) { stop_flag = 1; }

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

struct thread_data {
  int sock;
  char *packet;
  // int packet_len;
  struct sockaddr_ll sll;
};

void *sender_thread(void *arg) {
  struct thread_data *data = (struct thread_data *)arg;
  char input[MAX_INPUT_SIZE];

  while (!stop_flag) {
    printf("Enter message (or 'quit' to exit): ");
    fgets(input, MAX_INPUT_SIZE, stdin);
    input[strcspn(input, "\n")] = 0;  // Remove newline
    if (strcmp(input, "quit") == 0) {
      stop_flag = 1;
    }
    int data_len = strlen(input);

    struct iphdr *iph =
        (struct iphdr *)(data->packet + sizeof(struct ether_header));
    iph->tot_len =
        htons(sizeof(struct iphdr) + sizeof(struct udphdr) + data_len);
    iph->check = 0;
    iph->check = ip_checksum(iph);

    // Update packet payload with new message
    struct udphdr *udph =
        (struct udphdr *)(data->packet + sizeof(struct ether_header) +
                          sizeof(struct iphdr));
    char *payload = (char *)(udph + 1);
    memcpy(payload, input, data_len + 1);

    // Recalculate UDP length
    udph->len = htons(sizeof(struct udphdr) + data_len);

    int packet_tot_len = sizeof(struct ether_header) + sizeof(struct iphdr) +
                         sizeof(struct udphdr) + data_len;

    if (sendto(data->sock, data->packet, packet_tot_len, 0,
               (struct sockaddr *)&data->sll, sizeof(data->sll)) == -1) {
      perror("sendto");
      break;
    }
    printf("Sent: %s\n", input);
    usleep(500);
  }

  return NULL;
}

void *receiver_thread(void *arg) {
  int sock = *(int *)arg;
  char buf[MAX_INPUT_SIZE];

  in_addr_t client_ip, server_ip;
  inet_pton(AF_INET, "10.10.10.3", &client_ip);
  inet_pton(AF_INET, "10.10.10.2", &server_ip);

  while (!stop_flag) {
    int bytes_received = recv(sock, buf, sizeof(buf), 0);
    if (bytes_received == -1) {
      perror("recvfrom");
      break;
    }

    struct ether_header *eth = (struct ether_header *)buf;
    // if (memcmp(eth->ether_dhost, client_mac, 6) != 0) continue;
    // if (memcmp(eth->ether_shost, server_mac, 6) != 0) continue;
    if (ntohs(eth->ether_type) != ETHERTYPE_IP) continue;

    struct iphdr *iph = (struct iphdr *)(buf + sizeof(struct ether_header));
    if (iph->protocol != IPPROTO_UDP) continue;
    if (iph->daddr != client_ip) continue;
    if (iph->saddr != server_ip) continue;

    struct udphdr *udph =
        (struct udphdr *)((buf + iph->ihl * 4) + sizeof(struct ether_header));
    if (ntohs(udph->dest) != 7777) continue;
    if (ntohs(udph->source) != 9999) continue;

    char *payload = (char *)(udph + 1);
    uint16_t udp_len = ntohs(udph->len);
    uint16_t payload_len = udp_len - sizeof(struct udphdr);
    payload[payload_len] = '\0';
    printf("Received: %s\n", payload);
  }

  return NULL;
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

  char packet[1024] = {0};

  struct ether_header *eth = (struct ether_header *)packet;
  unsigned char src_mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x66};
  unsigned char dest_mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
  memcpy(eth->ether_shost, src_mac, 6);
  memcpy(eth->ether_dhost, dest_mac, 6);
  eth->ether_type = htons(ETHERTYPE_IP);

  struct iphdr *iph = (struct iphdr *)(packet + sizeof(struct ether_header));
  iph->version = 4;
  iph->ihl = 5;  // 5 * 4 = 20 bytes
  iph->tos = 0;
  // iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) +
  // data_len);
  iph->id = 0;
  iph->frag_off = 0;
  iph->ttl = 64;
  iph->protocol = IPPROTO_UDP;
  iph->check = 0;
  inet_pton(AF_INET, "10.10.10.3", &iph->saddr);
  inet_pton(AF_INET, "10.10.10.2", &iph->daddr);
  // iph->check = ip_checksum(iph);

  struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct ether_header) +
                                          sizeof(struct iphdr));
  // char *payload = (char *)(udph + 1);

  udph->source = htons(7777);
  udph->dest = htons(9999);
  // udph->len = htons(sizeof(struct udphdr) + data_len);
  udph->check = 0;

  // int packet_len = sizeof(struct ether_header) + sizeof(struct iphdr) +
  //                  sizeof(struct udphdr) + data_len;

  struct sockaddr_ll sll = {0};
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = if_nametoindex("eth0");
  // sll.sll_halen = ETH_ALEN;
  // memcpy(sll.sll_addr, dest_mac, 6);
  sll.sll_protocol = htons(ETH_P_IP);
  struct thread_data tdata = {.sock = s, .packet = packet, .sll = sll};

  pthread_t sender_tid, receiver_tid;

  // signal(SIGINT, handle_sigint);

  if (pthread_create(&sender_tid, NULL, sender_thread, &tdata) != 0) {
    perror("pthread_create sender");
    exit(EXIT_FAILURE);
  }
  if (pthread_create(&receiver_tid, NULL, receiver_thread, &s) != 0) {
    perror("pthread_create receiver");
    exit(EXIT_FAILURE);
  }

  pthread_join(sender_tid, NULL);
  pthread_join(receiver_tid, NULL);

  close(s);
}