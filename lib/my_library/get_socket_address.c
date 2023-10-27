#include "get_socket_address.h"

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

const size_t MY_INET_ADDRSTRLEN = INET_ADDRSTRLEN + 6;
const size_t MY_INET6_ADDRSTRLEN = INET6_ADDRSTRLEN + 8;

void GetSocketAddress(const struct sockaddr *addr, char buf[], size_t len) {
    memset(buf, 0, len);
    if(addr->sa_family == AF_INET) {
        assert(len >= MY_INET_ADDRSTRLEN);
        char buf0[INET_ADDRSTRLEN] = {};
        inet_ntop(AF_INET, &(((struct sockaddr_in *)addr)->sin_addr), buf0, INET_ADDRSTRLEN);
        uint16_t port = ntohs(((struct sockaddr_in *)addr)->sin_port);
        snprintf(buf, len, "%s:%d", buf0, port);
    } else if(addr->sa_family == AF_INET6) {
        assert(len >= MY_INET6_ADDRSTRLEN);
        char buf0[INET6_ADDRSTRLEN] = {};
        inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)addr)->sin6_addr), buf0, INET6_ADDRSTRLEN);
        uint16_t port = ntohs(((struct sockaddr_in6 *)addr)->sin6_port);
        snprintf(buf, len, "[%s]:%d", buf0, port);
    }
}
