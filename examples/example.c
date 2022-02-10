#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include <libstuncli/libstuncli.h>

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("Error: An interface name is required\n");
    exit(1);
  }

  // 1st Initialize the stun client
  stun_client_init("stun.l.google.com", 3478);

  // 2nd Obtain the public ipv4 of an interface
  struct in_addr ipv4;
  bool r = get_public_ipv4(argv[1], &ipv4);

  if (r) {
    printf("Ipv4: %s\n", inet_ntoa(ipv4));
  } else {
    printf("Error\n");
  }

  // 3rd After the request(s) destroy the stun client
  stun_client_destroy();

  return 0;
}
