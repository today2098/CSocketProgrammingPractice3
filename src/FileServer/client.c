#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
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
    char buf[BUF_SIZE / 2];
    ssize_t m, n;
    int res;

    // (1-1) socket(): ソケットを作成．
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, "socket()");

    // (1-2) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    res = inet_pton(AF_INET, "127.0.0.1", &server.sin_addr.s_addr);
    if(res <= 0) DieWithSystemMessage(__LINE__, "inet_pton()");

    // (1-3) connect(): サーバに接続．
    res = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if(res == -1) DieWithSystemMessage(__LINE__, "connect()");

    printf("connect to server\n");
    fflush(stdout);

    // 作業ディレクトリを移動．
    mkdir(OUTPUT_DIR_PATH, 0755);
    res = chdir(OUTPUT_DIR_PATH);
    if(res == -1) DieWithSystemMessage(__LINE__, "chdir()");

    while(1) {
        // (2-1) ファイル名を入力．
        char filename[NAME_MAX] = {};
        printf("What name of file are you looking for?> ");
        fflush(stdout);
        scanf("%s", filename);

        // (2-2) write(sock): ファイル名を送信．
        n = write(sock, filename, strlen(filename));
        if(n == -1) DieWithSystemMessage(__LINE__, "write()");

        // (2-3) read(sock): open()のerrnoを受信．
        int no;
        n = read(sock, &no, sizeof(no));
        if(n == 0) break;  // EOF.

        if(no != 0) {  // 失敗した場合．
            // [debug] エラー内容を表示．
            printf("%s\n", strerror(no));
            fflush(stdout);
        } else {  // 成功した場合．
            // (2-4) open(): 空ファイルを作成．
            int fd = open(filename, O_WRONLY | O_CREAT, 0755);
            if(fd == -1) DieWithSystemMessage(__LINE__, "open()");

            // (2-5) read(sock): データを受信．
            int cnt = 1;
            memset(buf, 0, sizeof(buf));
            while((m = read(sock, buf, sizeof(buf) - 1)) > 0) {
                fflush(stdout);
                n = write(fd, buf, m);

                printf("[%d] %ld %ld bytes\n", cnt++, m, n);
                if(n < m) DieWithSystemMessage(__LINE__, "write()");
            }
            if(m == -1) DieWithSystemMessage(__LINE__, "read()");
            printf("transmission complete\n");
            fflush(stdout);
        }
        if(m == 0) break;  // EOF.
        if(m == -1) DieWithSystemMessage(__LINE__, "read()");
        printf("transmission complete\n");
        fflush(stdout);

        // (2-6) 続けるかのフラグを入力．
        int flag = 1;
        printf("continue? [Yes: 0, No: 1]> ");
        fflush(stdout);
        scanf("%d", &flag);

        // (2-7) write(): 続けるかのフラグを送信．
        n = write(sock, &flag, sizeof(flag));
        if(n == -1) DieWithSystemMessage(__LINE__, "write()");

        if(flag) break;
    }

    // (2-8) close(): ソケットを閉じる．
    close(sock);

    return 0;
}
