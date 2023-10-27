#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/die_with_system_message.h"
#include "my_library/directory.h"
#include "protocol.h"

int main(int argc, char *argv[]) {
    char buf[BUF_SIZE];
    ssize_t m, n;
    int res;

    // (1) socket(): Listen用ソケットを作成．
    int sock0 = socket(PF_INET, SOCK_STREAM, 0);
    if(sock0 == -1) DieWithSystemMessage(__LINE__, "socket()");

    // (2) bind(): Listen用ソケットに名前付け．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    res = bind(sock0, (struct sockaddr *)&server, sizeof(server));
    if(res == -1) DieWithSystemMessage(__LINE__, "bind()");

    // (3) listen(): 接続要求を待ち受ける状態にする．
    res = listen(sock0, 5);
    if(res == -1) DieWithSystemMessage(__LINE__, "listen()");

    printf("listen now...\n");
    fflush(stdout);

    // (4) accept(): TCPクライアントからの接続要求を受け付ける．
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    socklen_t len = sizeof(client);
    int sock = accept(sock0, (struct sockaddr *)&client, &len);
    if(sock == -1) DieWithSystemMessage(__LINE__, "accept()");

    // [debug] クライアントのソケットアドレスを表示．
    char buf0[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &client.sin_addr, buf0, sizeof(buf0));
    printf("accept from %s:%u\n", buf0, ntohs(client.sin_port));
    fflush(stdout);

    // 作業ディレクトリを移動．
    res = chdir(DATA_DIR_PATH);
    if(res == -1) DieWithSystemMessage(__LINE__, "chdir()");

    // (5) read(sock): ファイル名を受信．
    memset(buf, 0, sizeof(buf));
    n = read(sock, buf, sizeof(buf));
    if(n == -1) DieWithSystemMessage(__LINE__, "read()");


    // [debug] 受信したファイル名を表示．
        printf("File \"%s\" requested\n", buf);
        fflush(stdout);

    // (6) open(): ファイルを開く．
    int fd = open(buf, O_RDONLY);
    int no = errno;

    if(no != 0) {  // 失敗した場合．
        // [debug] エラー内容を表示．
        printf("%s\n", strerror(no));
        fflush(stdout);

        // (2-3') write(sock): open()のerrnoを送信．
        n = write(sock, &no, sizeof(no));
        if(n == -1) DieWithSystemMessage(__LINE__, "write()");
    } else {
        // [debug]
        printf("find the file\n");
        fflush(stdout);

        // (2-3) write(sock): open()のerrnoを送信．
        n = write(sock, &no, sizeof(no));
        if(n == -1) DieWithSystemMessage(__LINE__, "write()");

        // (2-4) write(sock): データを送信．
        int cnt = 1;
        memset(buf, 0, sizeof(buf));
        while((m = read(fd, buf, sizeof(buf))) > 0) {
            n = write(sock, buf, m);
            if(n < m) DieWithSystemMessage(__LINE__, "write()");
            printf("[%d] %ld %ld bytes\n", cnt++, m, n);
            fflush(stdout);
        }
        if(m == 0) break;  // EOF.
        if(m == -1) DieWithSystemMessage(__LINE__, "read()");
        printf("transmission complete\n");
        fflush(stdout);
    }


    
    // (2-6) close(): TCPセッションを終了．
    close(sock);

    // (2-7) close(): Listen用socketを閉じる．
    close(sock0);

    return 0;
}
