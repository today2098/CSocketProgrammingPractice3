#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void Usage(char *argv[]) {
    fprintf(stderr,
            "Usage:   %s <hostname> <port number>\n"
            "Example: %s localhost 12345\n"
            "Message Transfer by TCP (client).\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        Usage(argv);
        return 1;
    }

    const char *hostname = argv[1];
    const char *port_str = argv[2];
    int ret;

    // (1) 名前解決の条件を指定．
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4．
    hints.ai_socktype = SOCK_STREAM;  // ストリームソケット (TCP)．

    // (2) getaddrinfo(): 指定されたホスト名から，名前解決を行う．
    struct addrinfo *result0;
    int status = getaddrinfo(hostname, port_str, &hints, &result0);
    if(status != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
        return 1;
    }

    // コネクションが成功するまでリンクリストを走査する．
    int sock = -1;
    for(struct addrinfo *result = result0; result; result = result->ai_next) {
        // (3) socket(): ソケットを作成．
        sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if(sock == -1) continue;

        // (4) connect(): サーバに接続．
        ret = connect(sock, result->ai_addr, result->ai_addrlen);
        if(ret == -1) {
            close(sock);
            sock = -1;
            continue;
        }

        // [debug] サーバのソケットアドレスを表示．
        char buf0[INET_ADDRSTRLEN] = {};
        inet_ntop(AF_INET, &(((struct sockaddr_in *)(result->ai_addr))->sin_addr.s_addr), buf0, sizeof(buf0));
        uint16_t port = ntohs(((struct sockaddr_in *)(result->ai_addr))->sin_port);
        printf("connect to %s:%d\n", buf0, port);
        fflush(stdout);
        break;
    }

    // (5) freeaddrinfo(): リンクリストresult0に割り当てられたメモリを解放する．
    freeaddrinfo(result0);

    if(sock == -1) {
        fprintf(stderr, "connection failed\n");
        return 1;
    }

    // (6) read(): メッセージを受信．
    char buf[1024];
    size_t offset;
    ssize_t n = 0;
    for(offset = 0; offset < sizeof(buf) - 1; offset += n) {
        n = read(sock, buf + offset, sizeof(buf) - 1 - offset);
        if(n <= 0) break;
    }
    if(n == -1) {
        perror("read()");
        return 1;
    }
    buf[offset] = '\0';

    // [debug] メッセージ内容を表示．
    printf("receive message\n");
    printf("    message: \"%s\"\n", buf);
    printf("    size:    %ld bytes\n", offset);
    fflush(stdout);

    // (7) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
