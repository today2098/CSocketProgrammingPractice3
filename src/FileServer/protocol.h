#ifndef C_SOCKET_PROGRAMMING_PRACTICE_FILE_SERVER_PROTOCOL_H
#define C_SOCKET_PROGRAMMING_PRACTICE_FILE_SERVER_PROTOCOL_H 1

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

// Port number of server. 16bits.
const uint16_t P_PORT = 12345;
const char *P_PORT_STR = "12345";

const size_t P_BUF_SIZE = 65536;         // Buffer size. 2^16.
const size_t P_FILENAME_MAX = NAME_MAX;  // Max size of filename. P_FILENAME_MAX < P_BUF_SIZE

#endif
