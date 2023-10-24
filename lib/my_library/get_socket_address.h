#ifndef C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_GET_SOCKET_ADDRESS_H
#define C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_GET_SOCKET_ADDRESS_H 1

#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

const size_t MY_INET_ADDRSTRLEN = INET_ADDRSTRLEN + 6;
const size_t MY_INET6_ADDRSTRLEN = INET6_ADDRSTRLEN + 8;

// Print socket address from struct sockaddr.
void GetSocketAddress(const struct sockaddr *addr, char buf[], size_t len);

#endif
