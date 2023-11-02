#include "get_socket_address.h"

#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

const size_t MY_INET_ADDRSTRLEN = INET_ADDRSTRLEN + 6;
const size_t MY_INET6_ADDRSTRLEN = INET6_ADDRSTRLEN + 8;

int GetSocketAddress(const struct sockaddr *saddr, char buf[], size_t len) {
    memset(buf, 0, len);
    if(saddr->sa_family == AF_INET) {
        if(len < MY_INET_ADDRSTRLEN) return -1;
        char buf0[INET_ADDRSTRLEN] = {};
        const char *p = inet_ntop(AF_INET, &(((struct sockaddr_in *)saddr)->sin_addr), buf0, INET_ADDRSTRLEN);
        if(p == NULL) {
            snprintf(buf, len, "[invalid address]");
            return -1;
        }
        uint16_t port = ntohs(((struct sockaddr_in *)saddr)->sin_port);
        snprintf(buf, len, "%s:%d", buf0, port);
    } else if(saddr->sa_family == AF_INET6) {
        if(len < MY_INET6_ADDRSTRLEN) return -1;
        char buf0[INET6_ADDRSTRLEN] = {};
        const char *p = inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)saddr)->sin6_addr), buf0, INET6_ADDRSTRLEN);
        if(p == NULL) {
            snprintf(buf, len, "[invalid address]");
            return -1;
        }
        uint16_t port = ntohs(((struct sockaddr_in6 *)saddr)->sin6_port);
        snprintf(buf, len, "[%s]:%d", buf0, port);
    } else {
        snprintf(buf, len, "[unknown type]");
    }
    return 0;
}
