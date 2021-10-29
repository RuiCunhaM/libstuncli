/**
 * @file stun.h
 *
 * @brief Stun client functions 
 *
 */

#include <stdbool.h>

/**
 * @brief Initialize stun client
 *
 * @param[in] stun_server_ip Stun server ip (literal or hostname)
 * @param[in] stun_server_port Stun server port
 *
 * @return true on success or false if an error occurs
 */
bool stun_server_init(char *stun_server_ip, short stun_server_port);

/**
 * @brief Destroy stun client
 */
void stun_server_destroy();

/**
 * @brief Get public IPv4 address 
 *
 * @param[in] interface_name Name of network interface to be used
 * @param[out] ipv4 On success IPv4 is written to this structure
 *
 * @return true on success or false if an error occurs
 */
bool get_public_ipv4(char *interface_name, struct in_addr *ipv4);
