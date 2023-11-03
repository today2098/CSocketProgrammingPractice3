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

// 引数のホスト名から名前解決を行い，IPアドレスの文字列を返す．
char *NameResolution(const char *hostname) {
    // (a-1) 名前解決の条件を指定．
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4．
    hints.ai_socktype = SOCK_STREAM;  // ストリームソケット (TCP)．

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
    printf("ip address of server is %s\n", ipaddr_str);
    fflush(stdout);

    // (2) socket(): ストリームソケット (TCP) を作成．
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (3) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(port_str));
    ret = inet_pton(AF_INET, ipaddr_str, &server.sin_addr.s_addr);
    if(ret == -1) {
        perror("inet_pton()");
        return 1;
    }
    if(ret == 0) {
        fprintf(stderr, "wrong network address notation\n");
        return 1;
    }

    // (4) connect(): サーバに接続．
    ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if(ret == -1) {
        perror("connect()");
        return 1;
    }

    printf("connect to %s:%s\n", ipaddr_str, port_str);
    fflush(stdout);

    // (5) read(): メッセージを受信．
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

    // (6) close(): ソケットを閉じる．
    close(sock);

    free(ipaddr_str);
    return 0;
}
