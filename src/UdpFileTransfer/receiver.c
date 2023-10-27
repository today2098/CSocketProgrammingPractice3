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

#include "my_library/directory.h"
#include "my_library/get_socket_address.h"
#include "my_library/handle_error.h"
#include "protocol.h"

int main() {
    char buf[BUF_SIZE];
    ssize_t m, n;
    int ret;

    // ディレクトリを移動．
    mkdir(OUTPUT_DIR_PATH, 0755);
    ret = chdir(OUTPUT_DIR_PATH);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "chdir()");

    // (1) socket(): UDPソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // (2) bind(): ソケットに名前付け．
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sock, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "bind()");

    // (3) read(): ファイル名を受信．
    struct sockaddr_in dst_saddr;
    memset(&dst_saddr, 0, sizeof(dst_saddr));
    socklen_t len = sizeof(dst_saddr);
    n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&dst_saddr, &len);
    if(n == -1) DieWithSystemMessage(__LINE__, errno, "recvfrom()");
    buf[n] = '\0';

    // [debug] ファイル名と送信元のソケットアドレスを表示．
    char buf0[MY_INET_ADDRSTRLEN];
    GetSocketAddress((struct sockaddr *)&dst_saddr, buf0, sizeof(buf0));
    printf("receive filename \"%s\" from %s\n", buf, buf0);
    fflush(stdout);

    // (4) open(): 空ファイルを作成．
    int fd = open(buf, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if(fd == -1) DieWithSystemMessage(__LINE__, errno, "open()");

    // (5) recv(): ファイルデータを受信．
    int cnt = 0;
    ssize_t sum = 0;
    while((m = read(sock, buf, sizeof(buf))) > 0) {
        sum += m;
        printf("[%d] %ld bytes (total: %ld bytes)\n", ++cnt, m, sum);
        fflush(stdout);
        n = write(fd, buf, m);
        if(n < m) DieWithSystemMessage(__LINE__, errno, "write()");
    }
    if(m == -1) DieWithSystemMessage(__LINE__, errno, "read()");

    printf("transmission complete\n");
    fflush(stdout);

    // (6) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
