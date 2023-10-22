#include <arpa/inet.h>
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

void Usage(char *argv[]) {
    fprintf(stderr,
            "Usage: %s <output filename>\n"
            "Receive file data by TCP.\n",
            argv[0]);
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        Usage(argv);
        return 1;
    }

    const char *output_filename = argv[1];
    char buf[BUF_SIZE];
    ssize_t m, n;
    int res;

    // ディレクトリを移動．
    mkdir(OUTPUT_DIR_PATH, 0755);
    res = chdir(OUTPUT_DIR_PATH);
    if(res == -1) DieWithSystemMessage(__LINE__, "chdir()");

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
    server.sin_port = htons(PORT);
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

    // (5) open(): 空ファイルを作成．
    int fd = open(output_filename, O_WRONLY | O_CREAT, 0755);
    if(fd == -1) DieWithSystemMessage(__LINE__, "open()");

    // (6) read(): ファイルデータを受信．
    int cnt = 0;
    ssize_t sum = 0;
    while((m = read(sock, buf, sizeof(buf))) > 0) {
        sum += m;
        printf("[%d] %ld bytes, sum: %ld bytes\n", ++cnt, m, sum);
        fflush(stdout);
        n = write(fd, buf, m);
        if(n < m) DieWithSystemMessage(__LINE__, "write()");
    }
    if(m == -1) DieWithSystemMessage(__LINE__, "read()");
    printf(
        "connection is closed\n"
        "transmission complete\n");
    fflush(stdout);

    // (7) close(): TCPセッションを終了．
    close(sock);

    // (8) close(): Listen用ソケットを閉じる．
    close(sock0);
    return 0;
}
