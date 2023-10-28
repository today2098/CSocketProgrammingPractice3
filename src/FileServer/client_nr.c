#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/directory.h"
#include "my_library/get_socket_address.h"
#include "my_library/handle_error.h"
#include "my_library/presentation_layer.h"
#include "protocol.h"

void Usage(char *argv[]) {
    fprintf(stderr,
            "Usage:   %s <hostname> [<port number>]\n"
            "Example: %s localhost 12345\n"
            "File Server by TCP (Client).\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
    if(!(2 <= argc && argc <= 3)) {
        Usage(argv);
        return 1;
    }

    const char *hostname = argv[1];
    const char *port_str = (argc == 3 ? argv[2] : P_PORT_STR);
    int ret;

    // ディレクトリを移動．
    mkdir(OUTPUT_DIR_PATH, 0755);
    ret = chdir(OUTPUT_DIR_PATH);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "chdir()");

    // (1) 名前解決を行う．
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

        // (2) socket(): ソケットを作成．
        sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if(sock == -1) {
            PrintSystemMessage(__LINE__, errno, "socket()");
            continue;
        }

        // (3) connect(): サーバに接続．
        ret = connect(sock, result->ai_addr, result->ai_addrlen);
        if(ret == -1) {
            PrintSystemMessage(__LINE__, errno, "connect()");
            close(sock);
            sock = -1;
            continue;
        }

        printf("connection successful\n");
        fflush(stdout);
    }
    freeaddrinfo(result0);
    if(sock == -1) {
        fprintf(stderr, "[error] line: %d, connection failed\n", __LINE__);
        return 1;
    }

    // (4) ファイルのダウンロードを行う．
    int fd;
    uint8_t flag;
    char buf[P_BUF_SIZE];
    size_t filesize, m, n, sum;
    int cnt;
    while(1) {
        // (4-1) 要求するファイル名の入力．
        printf("enter filename> ");
        fflush(stdout);
        memset(buf, 0, sizeof(buf));
        while(1) {
            char *s = fgets(buf, sizeof(buf), stdin);
            if(s != NULL && strcmp(buf, "\n") != 0) break;
        }
        if(buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = '\0';
        // ret = scanf("%s", buf);
        // if(ret == EOF) DieWithSystemMessage(__LINE__, errno, "scanf()");
        if(strlen(buf) > P_FILENAME_MAX) {
            printf("the filename is too long\n");
            continue;
        }

        // (4-2) ファイル名を送信．
        WriteByteString(sock, buf, P_FILENAME_MAX + 1);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "write()");

        printf("send filename\n");
        fflush(stdout);

        // (4-3) ファイルの有無を確認．
        ret = Read8bits(sock, &flag);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "read()");
        if(flag == 1) {
            printf("\"%s\" does not exit\n", buf);
            fflush(stdout);
            goto Next;
        }

        // (4-4) ファイルサイズを受信．
        ret = Read64bits(sock, &filesize);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "read()");

        printf("file size: %ld bytes\n", filesize);
        fflush(stdout);

        // (4-5) open(): 空ファイルを作成．
        fd = open(buf, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(fd == -1) DieWithSystemMessage(__LINE__, errno, "open()");

        // (4-6) ファイルデータを受信．
        cnt = sum = 0;
        while((m = read(sock, buf, sizeof(buf))) > 0) {
            sum += m;
            printf("[%d] %ld bytes (total: %ld bytes)\n", ++cnt, m, sum);
            fflush(stdout);
            n = write(fd, buf, m);
            if(n < m) DieWithSystemMessage(__LINE__, errno, "write()");
            if(sum >= filesize) break;
        }
        if(m == -1) DieWithSystemMessage(__LINE__, errno, "read()");
        if(m == 0) {
            printf("communication disconnected\n");
            break;
        }
        close(fd);

        printf("download completed\n");
        fflush(stdout);

    Next:
        // (4-7) 続けるかの確認．
        printf("do you want to continue? [Yes: 0, No: 1]> ");
        fflush(stdout);
        flag = 1;
        ret = scanf("%hhd", &flag);
        if(ret == EOF) DieWithSystemMessage(__LINE__, errno, "scanf()");
        ret = Write8bits(sock, flag);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "write()");
        if(flag == 1) break;
    }

    // (5) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
