#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int res;

    // (1) socket(): Listen用ソケットを作成．
    int sock0 = socket(PF_INET, SOCK_STREAM, 0);
    if(sock0 == -1) {
        perror("socket()");
        return 1;
    }

    // (2) bind(): Listen用ソケットに名前付け．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = INADDR_ANY;
    res = bind(sock0, (struct sockaddr *)&server, sizeof(server));
    if(res == -1) {
        perror("bind()");
        return 1;
    }

    // (3) listen(): 接続要求を待ち受ける状態にする．
    res = listen(sock0, 5);
    if(res == -1) {
        perror("listen()");
        return 1;
    }

    printf("listen now...\n");
    fflush(stdout);

    // (4) accept(): TCPクライアントからの接続要求を受け付ける．
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    socklen_t len = sizeof(client);
    int sock = accept(sock0, (struct sockaddr *)&client, &len);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // [debug] クライアントのソケットアドレスを表示．
    char buf0[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &client.sin_addr, buf0, sizeof(buf0));
    printf("accept from %s:%u\n", buf0, ntohs(client.sin_port));
    fflush(stdout);

    // (5) write(): メッセージを送信．
    char message[1024] = "Hello! You've successfully connected.";
    ssize_t n = write(sock, message, strlen(message));
    if(n == -1) {
        perror("write()");
        return 1;
    }

    printf("send message\n");
    fflush(stdout);

    // (6) close(sock): TCPセッションを終了．
    close(sock);

    // (7) close(sock0): Listen用ソケットを閉じる．
    close(sock0);
    return 0;
}