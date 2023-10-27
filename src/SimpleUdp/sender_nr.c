#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
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
            "Message Transfer by UDP\n",
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
    hints.ai_family = AF_INET;       // IPv4.
    hints.ai_socktype = SOCK_DGRAM;  // データグラムソケット (UDP)．

    // (2) getaddrinfo(): 指定されたホスト名から，名前解決を行う．
    struct addrinfo *result0;
    int status = getaddrinfo(hostname, port_str, &hints, &result0);
    if(status != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
        exit(1);
    }

    // [debug] 宛先のソケットアドレスを表示．
    char buf0[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &(((struct sockaddr_in *)(result0->ai_addr))->sin_addr.s_addr), buf0, sizeof(buf0));
    uint16_t port = ntohs(((struct sockaddr_in *)(result0->ai_addr))->sin_port);
    printf("socket address is %s:%d\n", buf0, port);
    fflush(stdout);

    // (3) socket(): ソケットを作成．
    int sock = socket(result0->ai_family, result0->ai_socktype, result0->ai_protocol);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (4) sendto(): メッセージを送信．
    char message[1024] = "Hello, world.";
    ssize_t n = sendto(sock, message, strlen(message), 0, result0->ai_addr, result0->ai_addrlen);
    if(n == -1) {
        perror("sendto()");
        return 1;
    }

    printf("send message");
    fflush(stdout);

    // (5) close(): ソケットを閉じる．
    close(sock);

    // (6) freeaddrinfo(): リンクリストresult0に割り当てられたメモリを解放する．
    freeaddrinfo(result0);
    return 0;
}
