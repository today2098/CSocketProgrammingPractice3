#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

void Usage(char *argv[]) {
    fprintf(stderr,
            "Usage: %s <hostname> [port number|service]\n"
            "Name Resolution.\n",
            argv[0]);
}

int main(int argc, char *argv[]) {
    if(!(2 <= argc && argc <= 3)) {
        Usage(argv);
        return 1;
    }

    const char *hostname = argv[1];                       // ホスト名．
    const char *port_str = (argc == 3 ? argv[2] : NULL);  // ポート番号またはサービス名．

    // (1) 名前解決の条件を指定．
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    // (2) getaddrinfo(): 指定されたホスト名とサービス名から，名前解決を行う．
    struct addrinfo *result0;
    int status = getaddrinfo(hostname, port_str, &hints, &result0);
    if(status != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
        return 1;
    }

    // (3) リンクリストの内容を表示する．
    int cnt = 0;
    struct addrinfo *result;
    for(result = result0; result; result = result->ai_next) {
        // アドレスファミリ．
        printf("[%d] address family: ", ++cnt);
        if(result->ai_family == AF_INET) printf("INET (IPv4)");
        else if(result->ai_family == AF_INET6) printf("INET6 (IPv6)");
        else printf("other (%d)", result->ai_family);
        printf("\n");

        // ソケットタイプ．
        printf("    socket type:    ");
        if(result->ai_socktype == SOCK_STREAM) printf("stream socket (TCP)");
        else if(result->ai_socktype == SOCK_DGRAM) printf("datagram socket (UDP)");
        else if(result->ai_socktype == SOCK_RAW) printf("raw socket");
        else printf("other (%d)", result->ai_socktype);
        printf("\n");

        // ソケットアドレス．
        if(result->ai_family == AF_INET) {
            char buf[INET_ADDRSTRLEN] = {};
            struct in_addr addr;
            addr.s_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr.s_addr;
            inet_ntop(AF_INET, &addr, buf, INET_ADDRSTRLEN);
            uint16_t port = ntohs(((struct sockaddr_in *)(result->ai_addr))->sin_port);
            printf("    ip address:     %s:%d\n", buf, port);
        } else if(result->ai_family == AF_INET6) {
            char buf[INET6_ADDRSTRLEN] = {};
            unsigned char *const p = ((struct sockaddr_in6 *)(result->ai_addr))->sin6_addr.s6_addr;
            inet_ntop(AF_INET6, p, buf, INET6_ADDRSTRLEN);
            uint16_t port = ntohs(((struct sockaddr_in6 *)(result->ai_addr))->sin6_port);
            printf("    ip address:     %s.%d\n", buf, port);
        }
    }

    // (4) リンクリストresult0に割り当てられたメモリを解放する．
    freeaddrinfo(result0);
    return 0;
}
