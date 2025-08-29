#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

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

int main() {
  int s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (s == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // int one = 1;
  // if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) == -1) {
  //   perror("setsockopt");
  //   exit(EXIT_FAILURE);
  // }

  // struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(9999)};
  // inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

  char *message = "hello!";
  int data_len = strlen(message) + 1;

  char packet[1024] = {0};

  struct ether_header *eth = (struct ether_header *)packet;
  unsigned char src_mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x66};
  unsigned char dest_mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
  memcpy(eth->ether_shost, src_mac, 6);
  memcpy(eth->ether_dhost, dest_mac, 6);
  eth->ether_type = htons(ETHERTYPE_IP);

  struct iphdr *iph = (struct iphdr *)(packet + sizeof(struct ether_header));
  iph->version = 4;
  iph->ihl = 5; // 5 * 4 = 20 bytes
  iph->tos = 0;
  iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + data_len);
  iph->id = 0;
  iph->frag_off = 0;
  iph->ttl = 64;
  iph->protocol = IPPROTO_UDP;
  iph->check = 0;
  inet_pton(AF_INET, "10.10.10.3", &iph->saddr);
  inet_pton(AF_INET, "10.10.10.2", &iph->daddr);
  iph->check = ip_checksum(iph);

  struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct ether_header) +
                                          sizeof(struct iphdr));
  char *payload = (char *)(udph + 1);

  udph->source = htons(7777);
  udph->dest = htons(9999);
  udph->len = htons(sizeof(struct udphdr) + data_len);
  udph->check = 0;

  memcpy(payload, message, data_len);

  int packet_len = sizeof(struct ether_header) + sizeof(struct iphdr) +
                   sizeof(struct udphdr) + data_len;

  struct sockaddr_ll sll = {0};
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = if_nametoindex("eth0");
  // sll.sll_halen = ETH_ALEN;
  // memcpy(sll.sll_addr, dest_mac, 6);
  sll.sll_protocol = htons(ETH_P_IP);

  if (sendto(s, packet, packet_len, 0, (struct sockaddr *)&sll, sizeof(sll)) ==
      -1) {
    perror("sendto");
    exit(EXIT_FAILURE);
  }

  printf("sent: %s\n", message);
  while (1) {
    char buf[1024];

    int bytes_received = recv(s, buf, sizeof(buf), 0);
    if (bytes_received == -1) {
      perror("recvfrom");
      exit(EXIT_FAILURE);
    }

    struct iphdr *iph = (struct iphdr *)(buf + sizeof(struct ether_header));
    struct udphdr *udph =
        (struct udphdr *)((buf + iph->ihl * 4) + sizeof(struct ether_header));

    if (iph->protocol != IPPROTO_UDP)
      continue;
    if (ntohs(udph->dest) != 7777)
      continue;
    if (ntohs(udph->source) != 9999)
      continue;

    printf("payload: %s\n", (char *)(udph + 1));
    break;
  }

  close(s);
}