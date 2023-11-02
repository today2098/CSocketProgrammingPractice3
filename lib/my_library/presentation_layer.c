#include "presentation_layer.h"

#include <arpa/inet.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

uint64_t htonll(uint64_t n) {
#if BYTE_ORDER == BIG_ENDIAN
    return n;
#else
    return (((uint64_t)htonl(n)) << 32) + htonl(n >> 32);
#endif
}

uint64_t ntohll(uint64_t n) {
#if BYTE_ORDER == BIG_ENDIAN
    return n;
#else
    return (((uint64_t)ntohl(n)) << 32) + ntohl(n >> 32);
#endif
}

int ReadByteString(int sock, char *buf, size_t len) {
    size_t offset;
    ssize_t n;
    for(offset = 0; offset < len; offset += n) {
        n = read(sock, buf + offset, len - offset);
        if(n <= 0) return -1;
    }
    return 0;
}

int Read8bits(int sock, uint8_t *buf) {
    ssize_t n = read(sock, buf, sizeof(uint8_t));
    if(n <= 0) return -1;
    return 0;
}

int Read16bits(int sock, uint16_t *buf) {
    size_t offset;
    ssize_t n;
    for(offset = 0; offset < sizeof(uint16_t); offset += n) {
        n = read(sock, buf + offset, sizeof(uint16_t) - offset);
        if(n <= 0) return -1;
    }
    *buf = ntohs(*buf);
    return 0;
}

int Read32bits(int sock, uint32_t *buf) {
    size_t offset;
    ssize_t n;
    for(offset = 0; offset < sizeof(uint32_t); offset += n) {
        n = read(sock, buf + offset, sizeof(uint32_t) - offset);
        if(n <= 0) return -1;
    }
    *buf = ntohl(*buf);
    return 0;
}

int Read64bits(int sock, uint64_t *buf) {
    size_t offset;
    ssize_t n;
    for(offset = 0; offset < sizeof(uint64_t); offset += n) {
        n = read(sock, buf + offset, sizeof(uint64_t) - offset);
        if(n <= 0) return -1;
    }
    *buf = ntohll(*buf);
    return 0;
}

int WriteByteString(int sock, const char *buf, size_t len) {
    size_t offset;
    ssize_t n;
    for(offset = 0; offset < len; offset += n) {
        n = write(sock, buf + offset, len - offset);
        if(n <= 0) return -1;
    }
    return 0;
}

int Write8bits(int sock, uint8_t buf) {
    ssize_t n = write(sock, &buf, sizeof(uint8_t));
    if(n <= 0) return -1;
    return 0;
}

int Write16bits(int sock, uint16_t buf) {
    size_t offset;
    ssize_t n;
    buf = htons(buf);
    for(offset = 0; offset < sizeof(uint16_t); offset += n) {
        n = write(sock, &buf + offset, sizeof(uint16_t) - offset);
        if(n <= 0) return -1;
    }
    return 0;
}

int Write32bits(int sock, uint32_t buf) {
    size_t offset;
    ssize_t n;
    buf = htonl(buf);
    for(offset = 0; offset < sizeof(uint32_t); offset += n) {
        n = write(sock, &buf + offset, sizeof(uint32_t) - offset);
        if(n <= 0) return -1;
    }
    return 0;
}

int Write64bits(int sock, uint64_t buf) {
    size_t offset;
    ssize_t n;
    buf = htonll(buf);
    for(offset = 0; offset < sizeof(uint64_t); offset += n) {
        n = write(sock, &buf + offset, sizeof(uint64_t) - offset);
        if(n <= 0) return -1;
    }
    return 0;
}
