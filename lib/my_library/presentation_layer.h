#ifndef C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_PRESENTATION_LAYER_H
#define C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_PRESENTATION_LAYER_H 1

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Convert the unsinged 64bits intenger from host byte order to network byte order.
 *
 * @param n the unsinged 64bits intenger by host byte order
 * @return The unsinged 64bits intenger by network byte order.
 */
uint64_t htonll(uint64_t n);

/**
 * @brief Convert the unsinged 64bits intenger from network byte order to host byte order.
 *
 * @param n the unsinged 64bits intenger by network byte order
 * @return The unsinged 64bits intenger by host byte order.
 */
uint64_t ntohll(uint64_t n);

int ReadByteString(int sock, char *buf, size_t len);
int Read8bits(int sock, uint8_t *buf);
int Read16bits(int sock, uint16_t *buf);
int Read32bits(int sock, uint32_t *buf);
int Read64bits(int sock, uint64_t *buf);

int WriteByteString(int sock, const char *buf, size_t len);
int Write8bits(int sock, uint8_t buf);
int Write16bits(int sock, uint16_t buf);
int Write32bits(int sock, uint32_t buf);
int Write64bits(int sock, uint64_t buf);

#endif
