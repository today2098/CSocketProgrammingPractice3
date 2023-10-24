#include "print_socket_address.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

void PrintSocketAddress(FILE *stream, const struct sockaddr *addr) {
    if(addr->sa_family == AF_INET) {
        char buf[INET_ADDRSTRLEN] = {};
        inet_ntop(AF_INET, &(((struct sockaddr_in *)addr)->sin_addr.s_addr), buf, INET_ADDRSTRLEN);
        uint16_t port = ntohs(((struct sockaddr_in *)addr)->sin_port);
        fprintf(stream, "socket address: %s:%d\n", buf, port);
    } else if(addr->sa_family == AF_INET6) {
        char buf[INET6_ADDRSTRLEN] = {};
        inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)addr)->sin6_addr.s6_addr), buf, INET6_ADDRSTRLEN);
        uint16_t port = ntohs(((struct sockaddr_in6 *)addr)->sin6_port);
        fprintf(stream, "socket address: [%s]:%d\n", buf, port);
    }
}
