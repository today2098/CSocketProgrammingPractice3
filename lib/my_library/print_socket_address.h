#ifndef C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_PRINT_SOCKET_ADDRESS_H
#define C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_PRINT_SOCKET_ADDRESS_H 1

#include <stdio.h>
#include <sys/socket.h>

// Print socket address from struct sockaddr.
void PrintSocketAddress(FILE *stream, const struct sockaddr *addr);

#endif
