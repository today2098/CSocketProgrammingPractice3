#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/directory.h"
#include "my_library/get_socket_address.h"
#include "my_library/handle_error.h"
#include "protocol.h"

void Usage(char *argv[]) {
    fprintf(stderr,
            "Usage: %s <filename>\n"
            "File Transfer by UDP (Sender).\n",
            argv[0]);
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        Usage(argv);
        return 1;
    }

    const char *filename = argv[1];
    const char *ipaddr = "127.0.0.1";
    // const unsigned int delay = 1000;  // 1000 [microsec] = 1 [msec].
    char buf[BUF_SIZE];
    ssize_t m, n;
    int ret;

    // ディレクトリを移動．
    ret = chdir(DATA_DIR_PATH);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "chdir()");

    // (1) open(): ファイルを開く．
    int fd = open(filename, O_RDONLY);
    if(fd == -1) DieWithSystemMessage(__LINE__, errno, "open()");

    // (2) socket(): UDPソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // (3) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in dst_saddr;
    memset(&dst_saddr, 0, sizeof(dst_saddr));
    dst_saddr.sin_family = AF_INET;
    dst_saddr.sin_port = htons(PORT);
    ret = inet_pton(AF_INET, ipaddr, &dst_saddr.sin_addr.s_addr);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "inet_pton()");
    if(ret == 0) {
        fprintf(stderr, "[error] line: %d, inet_pton(): wrong network address notation\n", __LINE__);
        return 1;
    }

    // [debug] 宛先のソケットアドレスを表示．
    char buf0[MY_INET_ADDRSTRLEN];
    GetSocketAddress((struct sockaddr *)&dst_saddr, buf0, sizeof(buf0));
    printf("socket address: %s\n", buf0);
    fflush(stdout);

    // (4) write(): ファイル名を送信．
    n = sendto(sock, filename, strlen(filename), 0, (struct sockaddr *)&dst_saddr, sizeof(dst_saddr));
    if(n == -1) DieWithSystemMessage(__LINE__, errno, "sendto()");

    printf("send file name\n");
    fflush(stdout);

    // (5) write(): ファイルデータを送信．
    int cnt = 0;
    ssize_t sum = 0;
    while((m = read(fd, buf, sizeof(buf))) > 0) {
        n = sendto(sock, buf, m, 0, (struct sockaddr *)&dst_saddr, sizeof(dst_saddr));
        if(n < m) DieWithSystemMessage(__LINE__, errno, "sendto()");
        sum += n;
        printf("[%d] %ld bytes (total: %ld bytes)\n", ++cnt, n, sum);
        fflush(stdout);
        // usleep(delay);
    }
    if(m == -1) DieWithSystemMessage(__LINE__, errno, "read()");
    // 長さ0のデータグラムを送信する．
    n = sendto(sock, buf, 0, 0, (struct sockaddr *)&dst_saddr, sizeof(dst_saddr));
    if(n == -1) DieWithSystemMessage(__LINE__, errno, "sendto()");

    printf("transmission complete\n");
    fflush(stdout);

    // (6) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
