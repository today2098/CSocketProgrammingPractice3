#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int res;

    // (1) socket(): UDPソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (2) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    res = inet_pton(AF_INET, "127.0.0.1", &server.sin_addr.s_addr);
    if(res == 0 || res == -1) {
        if(res == 0) fprintf(stderr, "wrong network address notation\n");
        else perror("inet_pton");
        return 1;
    }

    // (3) sendto(): メッセージを送信．
    char message[1024] = "Hello, world.";
    ssize_t n = sendto(sock, message, strlen(message), 0, (struct sockaddr *)&server, sizeof(server));
    if(n == -1) {
        perror("sendto()");
        return 1;
    }

    // [debug]
    printf("send message to 127.0.0.1:12345\n");

    // (4) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
