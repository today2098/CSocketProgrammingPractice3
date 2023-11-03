#include "handle_socket_address.h"

#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

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

int IsSocketAddressEqual(const struct sockaddr *saddr1, const struct sockaddr *saddr2) {
    if(saddr1 == NULL || saddr2 == NULL) return saddr1 == saddr2;
    if(saddr1->sa_family == saddr2->sa_family) {
        if(saddr1->sa_family == AF_INET) {
            const struct sockaddr_in *p1 = (struct sockaddr_in *)saddr1;
            const struct sockaddr_in *p2 = (struct sockaddr_in *)saddr2;
            return p1->sin_addr.s_addr == p2->sin_addr.s_addr && p1->sin_port == p2->sin_port;
        } else if(saddr1->sa_family == AF_INET6) {
            const struct sockaddr_in6 *p1 = (struct sockaddr_in6 *)saddr1;
            const struct sockaddr_in6 *p2 = (struct sockaddr_in6 *)saddr2;
            return memcmp(&p1->sin6_addr, &p2->sin6_addr, sizeof(struct in6_addr)) == 0 && p1->sin6_port == p2->sin6_port;
        }
    }
    return 0;
}
