#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/die_with_system_message.h"
#include "my_library/directory.h"
#include "protocol.h"

void Usage(char *argv[]) {
    fprintf(stderr,
            "Usage: %s <filename>\n"
            "Send file data by TCP.\n",
            argv[0]);
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        Usage(argv);
        return 1;
    }

    const char *dst_ipaddr = "127.0.0.1";
    const char *filename = argv[1];
    char buf[BUF_SIZE];
    ssize_t m, n;
    int res;

    // ディレクトリを移動．
    res = chdir(DATA_DIR_PATH);
    if(res == -1) DieWithSystemMessage(__LINE__, "chdir()");

    // (1) open(): ファイルを開く．
    int fd = open(filename, O_RDONLY);
    if(fd == -1) DieWithSystemMessage(__LINE__, "open()");

    // (2) socket(): TCPソケットを作成．
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (3) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    res = inet_pton(AF_INET, dst_ipaddr, &server.sin_addr.s_addr);
    if(res == 0 || res == -1) {
        if(res == 0) fprintf(stderr, "wrong network address notation\n");
        else perror("inet_pton");
        return 1;
    }

    // (4) connect(): サーバに接続．
    res = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if(res == -1) {
        perror("connect()");
        return 1;
    }

    printf("connect to %s:%d\n", dst_ipaddr, PORT);
    fflush(stdout);

    // (5) write(): ファイルデータを送信．
    int cnt = 0;
    ssize_t sum = 0;
    while((m = read(fd, buf, sizeof(buf))) > 0) {
        n = write(sock, buf, m);
        if(n < m) DieWithSystemMessage(__LINE__, "write()");
        sum += n;
        printf("[%d] %ld bytes, sum: %ld bytes\n", ++cnt, n, sum);
        fflush(stdout);
    }
    if(m == -1) DieWithSystemMessage(__LINE__, "read()");
    printf("transmission complete\n");
    fflush(stdout);

    // (6) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
