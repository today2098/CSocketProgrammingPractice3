#include <arpa/inet.h>
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

    // (2) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in peer_saddr;
    memset(&peer_saddr, 0, sizeof(peer_saddr));
    peer_saddr.sin_family = AF_INET;     // IPv4.
    peer_saddr.sin_port = htons(12345);  // ポート番号．
    // IPアドレスをテキスト形式からバイナリ形式に変換する．127.0.0.1はlocalhost.
    ret = inet_pton(AF_INET, "127.0.0.1", &peer_saddr.sin_addr.s_addr);
    if(ret == -1) {
        perror("inet_pton()");
        return 1;
    }
    if(ret == 0) {
        fprintf(stderr, "wrong network address notation\n");
        return 1;
    }

    // (3) sendto(): メッセージを送信．
    char message[1024] = "Hello, world.";
    ssize_t n = sendto(sock, message, strlen(message), 0, (struct sockaddr *)&peer_saddr, sizeof(peer_saddr));
    if(n == -1) {
        perror("sendto()");
        return 1;
    }

    // [debug]
    printf("send message to 127.0.0.1:12345\n");
    fflush(stdout);

    // (4) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
