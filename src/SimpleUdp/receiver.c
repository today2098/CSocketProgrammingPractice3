#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int ret;

    // (1) socket(): データグラムソケット (UDP) を作成．
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

    // (3) recvfrom(): メッセージを受信．
    char buf[1024];
    struct sockaddr_in peer_saddr;
    socklen_t saddr_len = sizeof(peer_saddr);
    ssize_t n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&peer_saddr, &saddr_len);
    if(n == -1) {
        perror("recvfrom()");
        return 1;
    }
    buf[n] = '\0';

    // [debug] 送信元のソケットアドレスとメッセージを表示．
    char buf0[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &peer_saddr.sin_addr, buf0, sizeof(buf0));
    printf("receive message from %s:%d\n", buf0, ntohs(peer_saddr.sin_port));
    printf("    message: \"%s\"\n", buf);
    printf("    size:    %ld bytes\n", n);
    fflush(stdout);

    // (4) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
