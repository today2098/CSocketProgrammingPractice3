#ifndef C_SOCKET_PROGRAMMING_PRACTICE_SIMPLE_MULTICAST_PROTOCOL_H
#define C_SOCKET_PROGRAMMING_PRACTICE_SIMPLE_MULTICAST_PROTOCOL_H 1

#include <stddef.h>
#include <stdint.h>

#include "my_library/common_port.h"

// Multicast IP address (224.0.0.0 ~ 239.255.255.255).
const char *P_MULTICAST_IPADDR = "239.192.1.2";

// Port number of receiver. 16bits.
const uint16_t P_PORT = MY_COMMON_PORT;
const char *P_PORT_STR = MY_COMMON_PORT_STR;

const size_t P_BUF_SIZE = 1024;  // Buffer size. 2^10.

#endif
