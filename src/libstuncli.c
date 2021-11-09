#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <libstuncli.h>

#define BUFFSIZE 1024

#define BINDING_REQUEST 0x0001
#define MAGIC_COOKIE 0x2112A442
#define XORMAPPEDADDRESS 0x0020
#define TIMEOUT 1 // in seconds
#define IPV4FAMILY 0x01

// Macro from RFC5389
#define IS_SUCCESS_RESP(msg_type) (((msg_type)&0x0110) == 0x0100)

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

static struct sockaddr_in servaddr;
static int sockfd;

bool stun_client_init(char *stun_server_ip, short stun_server_port) {
  struct timeval tv;

  // Create socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP

  if (sockfd < 0)
    return false;

  // Set server details
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(stun_server_port);
  if ((inet_pton(AF_INET, stun_server_ip, &servaddr.sin_addr)) != 1) {
    // inet_pton failed -> try host resolve
    char host[128];
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(hints));

    // Reduce results to IPv4
    hints.ai_family = AF_INET;

    if (getaddrinfo(stun_server_ip, NULL, &hints, &res))
      return false;

    if (getnameinfo(res->ai_addr, res->ai_addrlen, host, sizeof(host), NULL, 0,
                    NI_NUMERICHOST))
      return false;

    if (inet_pton(AF_INET, host, &servaddr.sin_addr) != 1)
      return false;
  }

  // Set timeout
  memset(&tv, 0, sizeof(tv));
  tv.tv_sec = TIMEOUT;
  if ((setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,
                  sizeof tv)) < 0)
    return false;

  return true;
}

void stun_client_destroy() { close(sockfd); }

bool get_public_ipv4(char *interface_name, struct in_addr *ipv4) {
  char buf[BUFFSIZE];
  ssize_t n;
  struct ifreq interface;
  struct stunHeader bindRequest;

  // Set network interface
  memset(&interface, 0, sizeof(interface));
  strncpy(interface.ifr_name, interface_name, IFNAMSIZ);
  if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, &interface,
                 sizeof(interface)) < 0)
    return false;

  // STUN bind request
  bindRequest.stun_message = htons(BINDING_REQUEST);
  bindRequest.message_length = htons(0x0000); // zero length
  bindRequest.magic_cookie = htonl(MAGIC_COOKIE);
  bindRequest.transacation_id[0] = htonl(0x63c7117e); // Random
  bindRequest.transacation_id[1] = htonl(0x0714278f);
  bindRequest.transacation_id[2] = htonl(0x5ded3221);

  // Send request
  n = sendto(sockfd, &bindRequest, sizeof(struct stunHeader), 0,
             (struct sockaddr *)&servaddr, sizeof(servaddr));

  if (n < 0)
    return false;

  // Read response
  n = recvfrom(sockfd, buf, BUFFSIZE, 0, NULL, 0);
  if (n < 0)
    return false;

  struct stunHeader *bindResponse = (struct stunHeader *)buf;

  // TODO: Should we validade the Transaction ID?
  if (IS_SUCCESS_RESP(bindResponse->stun_message)) {
    for (int i = 20; (i - 20) < ntohs(bindResponse->message_length); i += 4) {
      struct attributeHeader *attrHeader = (struct attributeHeader *)&buf[i];

      if (ntohs(attrHeader->type) == XORMAPPEDADDRESS) {
        struct xorHeader *xorHeader = (struct xorHeader *)&buf[i + 4];

        // Port
        // port = ntohs(xorHeader->x_port);
        // port ^= (MAGIC_COOKIE >> 16);

        if (ntohs(xorHeader->family) == IPV4FAMILY) { // Ipv4
          int ip = *(int *)&buf[i + 8];
          ip ^= htonl(MAGIC_COOKIE);
          ipv4->s_addr = ip;
          return true;
        }
      }
      // Advance to next attribute
      i += ntohs(attrHeader->length);
    }
  }
  return false;
}
