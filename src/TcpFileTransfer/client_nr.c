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
            "Send file data by TCP.\n",
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
    hints.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo(hostname, port_str, &hints, &result0);
    if(status != 0) {
        fprintf(stderr, "[error] line: %d, getaddrinfo(): %s\n", __LINE__, gai_strerror(status));
        return 1;
    }

    // コネクションが成功するまでリンクリストを走査する．
    int sock = -1;
    struct addrinfo *result;
    for(result = result0; result; result = result->ai_next) {
        // [debug] サーバのソケットアドレスを表示．
        char buf0[MY_INET6_ADDRSTRLEN];
        GetSocketAddress(result->ai_addr, buf0, MY_INET6_ADDRSTRLEN);
        printf("try to establish a connection to %s\n", buf0);
        fflush(stdout);

        // (3) socket(): ソケットを作成．
        sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if(sock == -1) {
            PrintSystemMessage(__LINE__, "socket()", errno);
            continue;
        }

        // (4) connect(): サーバに接続．
        ret = connect(sock, result->ai_addr, result->ai_addrlen);
        if(ret == -1) {
            PrintSystemMessage(__LINE__, "connect()", errno);
            close(sock);
            sock = -1;
            continue;
        }

        printf("connection successful\n");
        fflush(stdout);
    }
    freeaddrinfo(result0);
    if(sock == -1) {
        fprintf(stderr, "[error] connection failed\n");
        return 1;
    }

    // (5) write(): ファイルデータを送信．
    int cnt = 0;
    ssize_t sum = 0;
    while((m = read(fd, buf, sizeof(buf))) > 0) {
        n = write(sock, buf, m);
        if(n < m) DieWithSystemMessage(__LINE__, "write()", errno);
        sum += n;
        printf("[%d] %ld bytes (total: %ld bytes)\n", ++cnt, n, sum);
        fflush(stdout);
    }
    if(m == -1) DieWithSystemMessage(__LINE__, "read()", errno);

    printf("transmission complete\n");
    fflush(stdout);

    // (6) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
