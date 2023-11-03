#ifndef C_SOCKET_PROGRAMMING_PRACTICE_UDP_FILE_TRANSFER_PROTOCOL_H
#define C_SOCKET_PROGRAMMING_PRACTICE_UDP_FILE_TRANSFER_PROTOCOL_H 1

#include <stddef.h>
#include <stdint.h>

#include "my_library/common_port.h"

// Port number of receiver. 16bits.
const uint16_t P_PORT = MY_COMMON_PORT;
const char *P_PORT_STR = MY_COMMON_PORT_STR;

const size_t P_BUF_SIZE = 4096;  // Buffer size. 2^12.

#endif
