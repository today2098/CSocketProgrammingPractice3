#ifndef C_SOCKET_PROGRAMMING_PRACTICE_TCP_FILE_TRANSFER_PROTOCOL_H
#define C_SOCKET_PROGRAMMING_PRACTICE_TCP_FILE_TRANSFER_PROTOCOL_H 1

#include <stddef.h>
#include <stdint.h>

#include "my_library/common_port.h"

// Port number of server. 16bits.
const uint16_t P_PORT = MY_COMMON_PORT;
const char *P_PORT_STR = MY_COMMON_PORT_STR;

const size_t P_BUF_SIZE = 65536;  // Buffer size. 2^16.

#endif
