#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFSIZE 1024

#define MAGICCOOKIE 0x2112A442
#define BINDINGREQUEST 0x0001
#define BINDINGRESPONSE 0x0101
#define MESSAGELENGHT 0x0000

// Macros defined in RFC5389
#define IS_REQUEST(msg_type) (((msg_type)&0x0110) == 0x0000)
#define IS_INDICATION(msg_type) (((msg_type)&0x0110) == 0x0010)
#define IS_SUCCESS_RESP(msg_type) (((msg_type)&0x0110) == 0x0100)
#define IS_ERR_RESP(msg_type) (((msg_type)&0x0110) == 0x0110)

#define XORMAPPEDADDRESS 0x0020

#define IPv4 0x01
#define IPv6 0x02

struct stunHeader {
  short stun_message;
  short message_length;
  int magic_cookie;
  int transacation_id[3];
};

struct attributeHeader {
  short type;
  short length;
};

struct xorHeader {
  short family;
  short x_port;
};

int stun_xor_addr(char *stun_server_ip, short stun_server_port) {

  int sockfd;
  struct sockaddr_in servaddr;
  char *buf = malloc(sizeof(char) * BUFFSIZE);
  struct stunHeader *bindRequest = malloc(sizeof(struct stunHeader));
  short n;
  unsigned short port;

  // Create socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  inet_pton(AF_INET, stun_server_ip, &servaddr.sin_addr);
  servaddr.sin_port = htons(stun_server_port);

  bindRequest->stun_message = htons(BINDINGREQUEST);
  bindRequest->message_length = htons(MESSAGELENGHT);
  bindRequest->magic_cookie = htonl(MAGICCOOKIE);
  bindRequest->transacation_id[0] = htonl(0x63c7117e); // Random
  bindRequest->transacation_id[1] = htonl(0x0714278f);
  bindRequest->transacation_id[2] = htonl(0x5ded3221);

  n = sendto(sockfd, bindRequest, sizeof(struct stunHeader), 0,
             (struct sockaddr *)&servaddr, sizeof(servaddr));

  if (n <= 0) {
    perror("Error sending request\n");
    return n;
  }

  // We wait to make sure we received the response
  sleep(1);

  // TODO: This blocks if no response
  n = recvfrom(sockfd, buf, BUFFSIZE, 0, NULL, 0);
  if (n <= 0) {
    perror("Error receiving response\n");
    return n;
  }

  struct stunHeader *bindResponse = (struct stunHeader *)buf;

  // TODO: Should we validade the Transaction ID?
  if (IS_SUCCESS_RESP(bindResponse->stun_message)) {
    for (int i = 20; (i - 20) < ntohs(bindResponse->message_length); i += 4) {
      struct attributeHeader *attrHeader = (struct attributeHeader *)&buf[i];

      if (ntohs(attrHeader->type) == XORMAPPEDADDRESS) {
        struct xorHeader *xorHeader = (struct xorHeader *)&buf[i + 4];

        // Port
        port = ntohs(xorHeader->x_port);
        port ^= (MAGICCOOKIE >> 16);

        if (ntohs(xorHeader->family) == IPv4) {
          // IP
          int ip = *(int *)&buf[i + 8];
          ip ^= htonl(MAGICCOOKIE);
          struct in_addr ip_addr;
          ip_addr.s_addr = ip;

          printf("Port: %d\n", port);
          printf("Ip: %s\n", inet_ntoa(ip_addr));

        } else if (ntohs(xorHeader->family) == IPv6) {
          // TODO
        }
        break;
      }
      // Advance to next attribute
      i += ntohs(attrHeader->length);
    }
  } else {
    perror("Error Response");
  }
  close(sockfd);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    exit(1);
  }

  int r = stun_xor_addr(argv[1], atoi(argv[2]));
  if (r != 0)
    printf("Error");
}
