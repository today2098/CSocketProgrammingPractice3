#ifndef C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_GET_SOCKET_ADDRESS_H
#define C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_GET_SOCKET_ADDRESS_H 1

#include <stddef.h>
#include <sys/socket.h>

#define MY_INET_ADDRSTRLEN (INET_ADDRSTRLEN + 6)
#define MY_INET6_ADDRSTRLEN (INET6_ADDRSTRLEN + 8)

/**
 * @brief Get the socket address by string.
 *
 * @param saddr pointer to struct sockaddr
 * @param buf pointer to buffer
 * @param len size of buffer
 * @return 0 is returned if there was no error. On error, -1 is returned.
 */
int GetSocketAddress(const struct sockaddr *saddr, char buf[], size_t len);

#endif
