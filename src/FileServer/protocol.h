#ifndef FILE_SERVER_PROTOCOL_H
#define FILE_SERVER_PROTOCOL_H 1

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

const uint16_t PORT = 12345;  // Port number of file server. 16bits.

const size_t BUF_SIZE = 2048;  // Buffer size. 2^16.

struct Header {
    ssize_t data_size;
};

#endif
