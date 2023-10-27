#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
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
            "Usage:   %s <filename> <hostname> <port number>\n"
            "Example: %s data.txt localhost 12345\n"
            "File Transfer by UDP (Sender).\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
    if(argc != 4) {
        Usage(argv);
        return 1;
    }

    const char *filename = argv[1];
    const char *hostname = argv[2];
    const char *port_str = argv[3];
    // const unsigned int delay = 1000;  // 1000 [microsec] = 1 [msec].
    char buf[BUF_SIZE];
    ssize_t m, n;
    int ret;

    // ディレクトリを移動．
    ret = chdir(DATA_DIR_PATH);
    if(ret == -1) DieWithSystemMessage(__LINE__, "chdir()", errno);

    // (1) open(): ファイルを開く．
    int fd = open(filename, O_RDONLY);
    if(fd == -1) DieWithSystemMessage(__LINE__, "open()", errno);

    // (2) 名前解決を行う．
    struct addrinfo hints, *result0;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    int status = getaddrinfo(hostname, port_str, &hints, &result0);
    if(status != 0) {
        fprintf(stderr, "[error] line: %d, getaddrinfo(): %s\n", __LINE__, gai_strerror(status));
        return 1;
    }

    // [debug] 宛先のソケットアドレスを表示．
    // 今回はリンクリストの先頭にあるアドレスを用いる．
    char buf0[MY_INET6_ADDRSTRLEN];
    GetSocketAddress(result0->ai_addr, buf0, sizeof(buf0));
    printf("socket address: %s\n", buf0);
    fflush(stdout);

    // (3) socket(): UDPソケットを作成．
    int sock = socket(result0->ai_family, result0->ai_socktype, result0->ai_protocol);
    if(sock == -1) DieWithSystemMessage(__LINE__, "socket()", errno);

    // (4) write(): ファイル名を送信．
    n = sendto(sock, filename, strlen(filename), 0, result0->ai_addr, result0->ai_addrlen);
    if(n == -1) DieWithSystemMessage(__LINE__, "sendto()", errno);

    printf("send file name\n");
    fflush(stdout);

    // (5) write(): ファイルデータを送信．
    int cnt = 0;
    ssize_t sum = 0;
    while((m = read(fd, buf, sizeof(buf))) > 0) {
        n = sendto(sock, buf, m, 0, result0->ai_addr, result0->ai_addrlen);
        if(n < m) DieWithSystemMessage(__LINE__, "sendto()", errno);
        sum += n;
        printf("[%d] %ld bytes (total: %ld bytes)\n", ++cnt, n, sum);
        fflush(stdout);
        // usleep(delay);
    }
    if(m == -1) DieWithSystemMessage(__LINE__, "read()", errno);
    // サイズ0のデータグラムを送信する．
    n = sendto(sock, buf, 0, 0, result0->ai_addr, result0->ai_addrlen);
    if(n == -1) DieWithSystemMessage(__LINE__, "sendto()", errno);

    printf("transmission complete\n");
    fflush(stdout);

    // (6) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
