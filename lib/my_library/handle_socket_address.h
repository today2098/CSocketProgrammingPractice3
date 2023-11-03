#ifndef C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_HANDLE_SOCKET_ADDRESS_H
#define C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_HANDLE_SOCKET_ADDRESS_H 1

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

/**
 * @brief Determine whether two socket addresses are equal.
 *
 * @param saddr1 socket address 1
 * @param saddr2 socket address 2
 * @return 1 is returned if two addresses are equal. If not equal, 0 is returned.
 */
int IsSocketAddressEqual(const struct sockaddr *saddr1, const struct sockaddr *saddr2);

#endif
