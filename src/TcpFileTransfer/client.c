#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/directory.h"
#include "my_library/handle_error.h"
#include "protocol.h"

void Usage(char *argv[]) {
    fprintf(stderr,
            "Usage:   %s <filename>\n"
            "Example: %s data.txt\n"
            "File Transfer by TCP (client).\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        Usage(argv);
        return 1;
    }

    const char *filename = argv[1];
    const char *ipaddr_str = "127.0.0.1";
    char buf[P_BUF_SIZE];
    ssize_t m, n;
    int ret;

    // ディレクトリを移動．
    ret = chdir(MY_DATA_DIR_PATH);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "chdir()");

    // (1) open(): ファイルを開く．
    int fd = open(filename, O_RDONLY);
    if(fd == -1) DieWithSystemMessage(__LINE__, errno, "open()");

    // (2) socket(): ストリームソケットを作成．
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // (3) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(P_PORT);
    ret = inet_pton(AF_INET, ipaddr_str, &server.sin_addr.s_addr);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "inet_pton()");
    if(ret == 0) {
        fprintf(stderr, "[error] line: %d, inet_pton(): wrong network address notation\n", __LINE__);
        return 1;
    }

    // (4) connect(): サーバに接続．
    ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "connect()");

    printf("connect to %s:%d\n", ipaddr_str, P_PORT);
    fflush(stdout);

    // (5) write(): ファイルデータを送信．
    int cnt = 0;
    ssize_t sum = 0;
    while((m = read(fd, buf, sizeof(buf))) > 0) {
        n = write(sock, buf, m);
        if(n < m) DieWithSystemMessage(__LINE__, errno, "write()");
        sum += n;
        printf("[%d] %ld bytes (total: %ld bytes)\n", ++cnt, n, sum);
        fflush(stdout);
    }
    if(m == -1) DieWithSystemMessage(__LINE__, errno, "read()");

    printf("transmission complete\n");
    fflush(stdout);

    // (6) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
