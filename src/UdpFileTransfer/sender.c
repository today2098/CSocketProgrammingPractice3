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
            "Send file data by UDP.\n",
            argv[0]);
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        Usage(argv);
        return 1;
    }

    const char *dst_ipaddr = "127.0.0.1";
    const char *filename = argv[1];
    // const unsigned int delay = 1000;  // 1000 [microsec] = 1 [msec].
    char buf[BUF_SIZE];
    ssize_t m, n;
    int res;

    // ディレクトリを移動．
    res = chdir(DATA_DIR_PATH);
    if(res == -1) DieWithSystemMessage(__LINE__, "chdir()");

    // (1) open(): ファイルを開く．
    int fd = open(filename, O_RDONLY);
    if(fd == -1) DieWithSystemMessage(__LINE__, "open()");

    // (2) socket(): UDPソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, "socket()");

    // (3) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    res = inet_pton(AF_INET, dst_ipaddr, &server.sin_addr.s_addr);
    if(res == 0 || res == -1) {
        if(res == -1) DieWithSystemMessage(__LINE__, "inet_pton()");
        fprintf(stderr, "wrong network address notation\n");
        return 1;
    }

    // (4) write(): ファイル名を送信．
    n = sendto(sock, filename, strlen(filename), 0, (struct sockaddr *)&server, sizeof(server));
    if(n == -1) DieWithSystemMessage(__LINE__, "sendto()");

    // (5) write(): ファイルデータを送信．
    int cnt = 0;
    ssize_t sum = 0;
    while((m = read(fd, buf, sizeof(buf))) > 0) {
        n = sendto(sock, buf, m, 0, (struct sockaddr *)&server, sizeof(server));
        if(n < m) DieWithSystemMessage(__LINE__, "sendto()");
        sum += n;
        printf("[%d] %ld bytes, sum: %ld bytes\n", ++cnt, n, sum);
        fflush(stdout);
        // usleep(delay);
    }
    if(m == -1) DieWithSystemMessage(__LINE__, "read()");
    // 長さ0のデータグラムを送信する．
    n = sendto(sock, buf, 0, 0, (struct sockaddr *)&server, sizeof(server));
    if(n == -1) DieWithSystemMessage(__LINE__, "sendto()");
    printf("transmission complete\n");
    fflush(stdout);

    // (6) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
