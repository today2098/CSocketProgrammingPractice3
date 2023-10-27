#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int ret;

    // (1) socket(): UDPソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (2) bind(): ソケットに名前付け．
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;          // IPv4.
    saddr.sin_port = htons(12345);       // ポート番号．
    saddr.sin_addr.s_addr = INADDR_ANY;  // IPアドレス（ワイルドカード）．
    ret = bind(sock, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind()");
        return 1;
    }

    // (3) recv(): メッセージを受信．
    char buf[1024];
    ssize_t n = read(sock, buf, sizeof(buf) - 1);
    if(n == -1) {
        perror("read()");
        return 1;
    }
    buf[n] = '\0';

    // [debug] メッセージを表示．
    printf("receive message\n");
    printf("  message: %s\n", buf);
    printf("  size: %ld bytes\n", n);
    fflush(stdout);

    // (4) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
