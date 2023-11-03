#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/directory.h"
#include "my_library/handle_error.h"
#include "my_library/handle_socket_address.h"
#include "protocol.h"

int main() {
    char buf[P_BUF_SIZE];
    ssize_t m, n;
    int ret;

    // ディレクトリを移動．
    mkdir(MY_OUTPUT_DIR_PATH, 0755);
    ret = chdir(MY_OUTPUT_DIR_PATH);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "chdir()");

    // (1) socket(): データグラムソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // (2) bind(): ソケットに名前付け．
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(P_PORT);
    saddr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sock, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "bind()");

    // (3) recvfrom(): ファイル名を受信．
    struct sockaddr_in peer_saddr;
    socklen_t saddr_len = sizeof(peer_saddr);
    n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&peer_saddr, &saddr_len);
    if(n == -1) DieWithSystemMessage(__LINE__, errno, "recvfrom()");
    buf[n] = '\0';

    // [debug] 送信元のソケットアドレスとファイル名を表示．
    char buf0[MY_INET_ADDRSTRLEN];
    GetSocketAddress((struct sockaddr *)&peer_saddr, buf0, sizeof(buf0));
    printf("receive filename \"%s\" from %s\n", buf, buf0);
    fflush(stdout);

    // (4) connect(): 受信する送信元を固定する．
    ret = connect(sock, (struct sockaddr *)&peer_saddr, saddr_len);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "connect");

    // (5) open(): 空ファイルを作成．
    int fd = open(buf, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if(fd == -1) DieWithSystemMessage(__LINE__, errno, "open()");

    // (6) read(sock): ファイルデータを受信．
    int cnt = 0;
    ssize_t sum = 0;
    while((m = read(sock, buf, sizeof(buf))) > 0) {
        sum += m;
        printf("[%d] %ld bytes (total: %ld bytes)\n", ++cnt, m, sum);
        fflush(stdout);

        // (7) write(fd): ファイルに書き込む．
        n = write(fd, buf, m);
        if(n < m) DieWithSystemMessage(__LINE__, errno, "write()");
    }
    if(m == -1) DieWithSystemMessage(__LINE__, errno, "read()");

    printf("transmission complete\n");
    fflush(stdout);

    // (8) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
