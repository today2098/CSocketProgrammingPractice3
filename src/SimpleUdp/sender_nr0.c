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

// ホスト名から名前解決を行い，IPアドレスの文字列を返す．
char *NameResolution(const char *hostname) {
    // (a-1) 名前解決の条件を指定．
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4.
    hints.ai_socktype = SOCK_DGRAM;  // データグラムソケット (UDP)．

    // (a-2) getaddrinfo(): 指定されたホスト名から，名前解決を行う．
    struct addrinfo *result0;
    int status = getaddrinfo(hostname, NULL, &hints, &result0);
    if(status != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
        exit(1);
    }

    // (a-3) inet_ntop(): リンクリストの先頭を用いて，IPアドレスを文字列に変換する．
    char *res = malloc(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(((struct sockaddr_in *)(result0->ai_addr))->sin_addr), res, INET_ADDRSTRLEN);

    // (a-4) リンクリストresult0に割り当てられたメモリを解放する．
    freeaddrinfo(result0);
    return res;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        Usage(argv);
        return 1;
    }

    const char *hostname = argv[1];
    const char *port_str = argv[2];
    int ret;

    // (1) 名前解決を行い，IPアドレスの文字列を取得する．
    char *ipaddr_str = NameResolution(hostname);
    printf("ip address: %s\n", ipaddr_str);
    fflush(stdout);

    // (2) socket(): UDPソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (3) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in dst_saddr;
    memset(&dst_saddr, 0, sizeof(dst_saddr));
    dst_saddr.sin_family = AF_INET;
    dst_saddr.sin_port = htons(atoi(port_str));
    ret = inet_pton(AF_INET, ipaddr_str, &dst_saddr.sin_addr.s_addr);
    if(ret == -1) {
        perror("inet_pton()");
        return 1;
    }
    if(ret == 0) {
        fprintf(stderr, "wrong network address notation\n");
        return 1;
    }

    // (4) sendto(): メッセージを送信．
    char message[1024] = "Hello, world.";
    ssize_t n = sendto(sock, message, strlen(message), 0, (struct sockaddr *)&dst_saddr, sizeof(dst_saddr));
    if(n == -1) {
        perror("sendto()");
        return 1;
    }

    // [debug]
    printf("send message to %s:%s\n", ipaddr_str, port_str);
    fflush(stdout);

    // (5) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
