#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "stun.h"

int main(int argc, char **argv) {

  if (argc < 2) {
    exit(1);
  }

  stun_server_init("stun.l.google.com", 3478);

  struct in_addr ipv4;
  bool r = get_public_ipv4(argv[1], &ipv4);

  if (r) {
    printf("Ipv4: %s\n", inet_ntoa(ipv4));
  } else {
    printf("Error\n");
  }

  stun_server_destroy();

  return 0;
}
