#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/get_socket_address.h"
#include "my_library/handle_error.h"
#include "protocol.h"

void Usage(char *argv[]) {
    fprintf(stderr,
            "Usage:   %s <hostname> <port number>\n"
            "Example: %s localhost 12345\n"
            "Message Transfer by TCP (Client).\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        Usage(argv);
        return 1;
    }

    const char *hostname = argv[1];
    const char *port_str = argv[2];
    char buf[BUF_SIZE];
    ssize_t n;
    int ret;

    // (1) 名前解決を行う．
    struct addrinfo hints, *result0;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo(hostname, port_str, &hints, &result0);
    if(status != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
        return 1;
    }

    // コネクションが成功するまでリンクリストを走査する．
    int sock = -1;
    struct addrinfo *result;
    for(result = result0; result; result = result->ai_next) {
        // [debug] サーバのソケットアドレスを表示．
        char buf0[MY_INET6_ADDRSTRLEN];
        GetSocketAddress(result->ai_addr, buf0, MY_INET6_ADDRSTRLEN);
        printf("try to establish a connection to %s\n", buf0);
        fflush(stdout);

        // (2) socket(): ソケットを作成．
        sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if(sock == -1) {
            PrintSystemMessage(__LINE__, "socket()", errno);
            continue;
        }

        // (3) connect(): サーバに接続．
        ret = connect(sock, result->ai_addr, result->ai_addrlen);
        if(ret == -1) {
            PrintSystemMessage(__LINE__, "connect()", errno);
            close(sock);
            sock = -1;
            continue;
        }

        printf("connection successful\n");
        fflush(stdout);
    }
    freeaddrinfo(result0);
    if(sock == -1) {
        fprintf(stderr, "[error] connection failed\n");
        return 1;
    }

    // (4) fgets(): 文字列を入力．
    printf("enter message> ");
    fflush(stdout);
    while(1) {
        char *s = fgets(buf, sizeof(buf), stdin);
        fflush(stdout);
        if(s != NULL && strcmp(buf, "\n") != 0) break;
    }
    if(buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = '\0';

    // (5) write(): 文字列を送信．
    n = write(sock, buf, strlen(buf));
    if(n < strlen(buf)) DieWithSystemMessage(__LINE__, "write()", errno);

    // [debug]
    printf("send message\n");
    printf("  message: \"%s\"\n", buf);
    printf("  size:    %ld bytes\n", n);
    fflush(stdout);

    // (6) read(): 変更された文字列を受信．
    n = read(sock, buf, sizeof(buf) - 1);
    if(n == -1) DieWithSystemMessage(__LINE__, "read()", errno);
    buf[n] = '\0';

    // [debug]
    printf("receive message\n");
    printf("  message: \"%s\"\n", buf);
    printf("  size:    %ld bytes\n", n);
    fflush(stdout);

    // (8) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
