#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int ret;

    // (1) socket(): Listen用ソケットを作成．
    int sock0 = socket(PF_INET, SOCK_STREAM, 0);
    if(sock0 == -1) {
        perror("socket()");
        return 1;
    }

    // setsockopt(): ソケット名の重複利用に関する制限を緩める．
    // "bind(): Address already in use" のエラー対策．
    int val = 1;
    ret = setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
    if(ret == -1) {
        perror("setsockopt()");
        return -1;
    }

    // (2) bind(): Listen用ソケットに名前付け．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;          // IPv4.
    server.sin_port = htons(12345);       // ポート番号．
    server.sin_addr.s_addr = INADDR_ANY;  // IPアドレス（ワイルドカード）．
    ret = bind(sock0, (struct sockaddr *)&server, sizeof(server));
    if(ret == -1) {
        perror("bind()");
        return 1;
    }

    // (3) listen(): 待ち行列を作成し，接続要求の受け付けを開始する．
    ret = listen(sock0, 5);
    if(ret == -1) {
        perror("listen()");
        return 1;
    }

    printf("listen now...\n");
    fflush(stdout);

    // (4) accept(): TCPクライアントからの接続要求を受け入れる．
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    socklen_t len = sizeof(client);  // in-out parameter.
    int sock = accept(sock0, (struct sockaddr *)&client, &len);
    if(sock == -1) {
        perror("accept()");
        return 1;
    }

    // [debug] クライアントのソケットアドレスを表示．
    char buf0[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &client.sin_addr, buf0, sizeof(buf0));
    printf("accept from %s:%u\n", buf0, ntohs(client.sin_port));
    fflush(stdout);

    // (5) write(): メッセージを送信．
    char msg[1024] = "Congratulations! You've successfully connected.";
    size_t offset;
    ssize_t n;
    for(offset = 0; offset < strlen(msg); offset += n) {
        n = write(sock, msg, strlen(msg) - offset);
        if(n <= 0) {
            perror("write()");
            return 1;
        }
    }

    printf("send message\n");
    fflush(stdout);

    // (6) close(sock): TCPセッションを終了．
    close(sock);

    // (7) close(sock0): Listen用ソケットを閉じる．
    close(sock0);
    return 0;
}
