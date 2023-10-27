#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int ret;

    // (1) socket(): TCPソケットを作成．
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (2) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;     // IPv4.
    server.sin_port = htons(12345);  // ポート番号．
    // IPアドレスをテキスト形式からバイナリ形式に変換する．127.0.0.1はlocalhost.
    ret = inet_pton(AF_INET, "127.0.0.1", &server.sin_addr.s_addr);
    if(ret == -1) {
        perror("inet_pton()");
        return 1;
    }
    if(ret == 0) {
        fprintf(stderr, "wrong network address notation\n");
        return 1;
    }

    // (3) connect(): サーバに接続．
    ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if(ret == -1) {
        perror("connect()");
        return 1;
    }

    printf("connect to 127.0.0.1:12345\n");
    fflush(stdout);

    // (4) read(): メッセージを受信．
    char buf[1024];
    ssize_t n = read(sock, buf, sizeof(buf) - 1);
    if(n == -1) {
        perror("read()");
        return 1;
    }
    buf[n] = '\0';

    // [debug] メッセージ内容を表示．
    printf("receive message\n");
    printf("  message: %s\n", buf);
    printf("  size:    %ld bytes\n", n);
    fflush(stdout);

    // (5) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
