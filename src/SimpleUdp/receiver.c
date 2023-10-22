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

    // (2) bind(): ソケットに名前付け．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = INADDR_ANY;
    res = bind(sock, (struct sockaddr *)&server, sizeof(server));
    if(res == -1) {
        perror("bind()");
        return 1;
    }

    // (3) recv(): メッセージを受信．
    char buf[1024];
    ssize_t n = read(sock, buf, sizeof(buf) - 1);
    buf[n] = '\0';
    if(n == -1) {
        perror("read()");
        return 1;
    }

    // [debug] メッセージを表示．
    printf("receive message\n");
    printf("  message: %s\n", buf);
    printf("  size: %ld bytes\n", n);

    // (4) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
