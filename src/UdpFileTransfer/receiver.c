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

int main() {
    char buf[BUF_SIZE];
    ssize_t m, n;
    int res;

    // (1) socket(): UDPソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, "socket()");

    // (2) bind(): ソケットに名前付け．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = INADDR_ANY;
    res = bind(sock, (struct sockaddr *)&server, sizeof(server));
    if(res == -1) DieWithSystemMessage(__LINE__, "bind()");

    // ディレクトリを移動．
    mkdir(OUTPUT_DIR_PATH, 0755);
    res = chdir(OUTPUT_DIR_PATH);
    if(res == -1) DieWithSystemMessage(__LINE__, "chdir()");

    // (3) read(): ファイル名を受信．
    memset(buf, 0, sizeof(buf));
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    socklen_t len = sizeof(client);
    n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&client, &len);
    if(n == -1) DieWithSystemMessage(__LINE__, "recvfrom()");

    // [debug] ファイル名を表示．
    char buf0[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &client.sin_addr, buf0, sizeof(buf0));
    printf("file \"%s\" will be transfered from %s:%d\n", buf, buf0, ntohs(client.sin_port));
    fflush(stdout);

    // (4) open(): 空ファイルを作成．
    int fd = open(buf, O_WRONLY | O_CREAT, 0755);
    if(fd == -1) DieWithSystemMessage(__LINE__, "open()");

    // (5) recv(): ファイルデータを受信．
    int cnt = 0;
    while((m = read(sock, buf, sizeof(buf) - 1)) > 0) {
        printf("[%d] %ld bytes\n", ++cnt, m);
        fflush(stdout);
        n = write(fd, buf, m);
        if(n < m) DieWithSystemMessage(__LINE__, "write()");
    }
    if(m == -1) DieWithSystemMessage(__LINE__, "read()");
    printf("transmission complete\n");
    fflush(stdout);

    // (6) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
