#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/directory.h"
#include "my_library/get_socket_address.h"
#include "my_library/handle_error.h"
#include "my_library/presentation_layer.h"
#include "protocol.h"

int main(int argc, char *argv[]) {
    int ret;

    // ディレクトリを移動．
    ret = chdir(DATA_DIR_PATH);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "chdir()");

    // (1) socket(): Listen用ソケットを作成．
    int sock0 = socket(PF_INET, SOCK_STREAM, 0);
    if(sock0 == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // (2) bind(): Listen用ソケットに名前付け．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(P_PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sock0, (struct sockaddr *)&server, sizeof(server));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "bind()");

    // (3) listen(): 接続要求の受け付けを開始．
    ret = listen(sock0, 5);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "listen()");

    printf("listen now...\n");
    fflush(stdout);

    // (4) accept(): TCPクライアントからの接続要求を受け入れる．
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    socklen_t slen = sizeof(client);
    int sock = accept(sock0, (struct sockaddr *)&client, &slen);
    if(sock == -1) DieWithSystemMessage(__LINE__, errno, "accept()");

    // [debug] クライアントのソケットアドレスを表示．
    char buf0[MY_INET_ADDRSTRLEN];
    GetSocketAddress((struct sockaddr *)&client, buf0, sizeof(buf0));
    printf("accept from %s\n", buf0);
    fflush(stdout);

    // (5) 要求されるファイルの送信を行う．
    struct stat sb;
    int fd;
    uint8_t flag;
    char buf[P_BUF_SIZE];
    size_t m, n, sum;
    int cnt;
    while(1) {
        // (5-1) ファイル名を受信．
        ret = ReadByteString(sock, buf, P_FILENAME_MAX + 1);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "read()");

        printf("requested file: %s\n", buf);
        fflush(stdout);

        // (5-2) open(): ファイルを開く．
        fd = open(buf, O_RDONLY);
        flag = (fd == -1 ? 1 : 0);

        // (5-3) ファイルの有無を確認．
        ret = Write8bits(sock, flag);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "write()");
        if(flag == 1) {
            printf("\"%s\" does not exit\n", buf);
            fflush(stdout);
            goto Next;
        }

        // (5-4) ファイルサイズを送信．
        memset(&sb, 0, sizeof(sb));
        ret = fstat(fd, &sb);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "fstat()");
        ret = Write64bits(sock, sb.st_size);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "write()");

        printf("file size: %ld bytes\n", sb.st_size);
        fflush(stdout);

        // (5-5) read(): ファイルデータを受信．
        cnt = sum = 0;
        while((m = read(fd, buf, sizeof(buf))) > 0) {
            n = write(sock, buf, m);
            if(n < m) DieWithSystemMessage(__LINE__, errno, "write()");
            sum += n;
            printf("[%d] %ld bytes (total: %ld bytes)\n", ++cnt, n, sum);
            fflush(stdout);
        }
        if(m == -1) DieWithSystemMessage(__LINE__, errno, "read()");
        close(fd);

        printf("transmission complete\n");
        fflush(stdout);

    Next:
        // (5-6) 続けるかを確認．
        ret = Read8bits(sock, &flag);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "read()");
        if(flag == 1) break;

        printf("continue\n");
    }

    printf("finish\n");
    fflush(stdout);

    // (6) close(): TCPセッションを終了．
    close(sock);

    // (7) close(): Listen用ソケットを閉じる．
    close(sock0);
    return 0;
}
