#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/handle_error.h"
#include "my_library/handle_socket_address.h"
#include "protocol.h"

void Usage(char *argv[]) {
    fprintf(stderr,
            "Usage:   %s [broadcast IP address] [port number]\n"
            "Example: %s 192.168.1.255 12345\n"
            "Broadcast Message (sender).\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
    if(!(1 <= argc && argc <= 3)) {
        Usage(argv);
        return 1;
    }

    const char *bcast_ipaddr_str = (argc >= 2 ? argv[1] : NULL);
    const char *port_str = (argc == 3 ? argv[2] : P_PORT_STR);
    int ret;

    // (1) socket(): データグラムソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // (2) setsockopt(): ブロードキャストを有効にする．
    int val = 1;
    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "setsockopt()");

    // (3) 宛先指定用のアドレス構造体を用意．
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(port_str));
    if(argc == 2) {
        ret = inet_pton(AF_INET, bcast_ipaddr_str, &saddr.sin_addr.s_addr);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "inet_pton()");
        if(ret == 0) {
            fprintf(stderr, "[error] line: %d, inet_pton(): wrong network address notation\n", __LINE__);
            return 1;
        }
    } else {
        saddr.sin_addr.s_addr = INADDR_BROADCAST;  // ローカルのネットワークへ．
    }
    socklen_t saddr_len = sizeof(saddr);

    // [debug]
    char buf0[MY_INET_ADDRSTRLEN];
    GetSocketAddress((struct sockaddr *)&saddr, buf0, sizeof(buf0));

    // (4) sendto(): 一定間隔でメッセージをブロードキャストする．
    const char *base_msg = "good morning! Hello everyone!! \\(0 v 0)/";
    char msg[P_BUF_SIZE];
    for(int i = 0;; ++i) {
        snprintf(msg, P_BUF_SIZE, "%dth %s", i + 1, base_msg);
        ssize_t n = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&saddr, saddr_len);
        if(n < strlen(msg)) DieWithSystemMessage(__LINE__, errno, "sendto()");

        // [debug]
        printf("[%d] broadcast to %s\n", i, buf0);
        printf("        message: \"%s\"\n", msg);
        printf("        size:    %ld bytes\n", strlen(msg));
        fflush(stdout);

        sleep(3);  // ネットワークがいっぱいになるのを防ぐ．
    }

    // (5) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
