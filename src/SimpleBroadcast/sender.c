#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/get_socket_address.h"
#include "my_library/handle_error.h"
#include "protocol.h"

void Usage(char *argv[]) {
    fprintf(stderr,
            "Usage:   %s [bcast ip addr]\n"
            "Example: %s 192.168.1.255\n"
            "Broadcast Message (sender).\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
    if(!(1 <= argc && argc <= 2)) {
        Usage(argv);
        return 1;
    }

    const char *bcast_ipaddr_str = (argc == 2 ? argv[1] : NULL);
    int ret;

    // (1) socket(): ソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // (2) setsockopt(): ブロードキャストを有効に設定．
    int val = 1;
    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "setsockopt()");

    // (3) 宛先指定用のアドレス構造体を用意．
    struct sockaddr_in dst_saddr;
    memset(&dst_saddr, 0, sizeof(dst_saddr));
    dst_saddr.sin_family = AF_INET;
    dst_saddr.sin_port = htons(P_PORT);
    if(argc == 2) {
        ret = inet_pton(AF_INET, bcast_ipaddr_str, &dst_saddr.sin_addr.s_addr);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "inet_pton()");
        if(ret == 0) {
            fprintf(stderr, "[error] line: %d, inet_pton(): wrong network address notation\n", __LINE__);
            return 1;
        }
    } else {
        dst_saddr.sin_addr.s_addr = INADDR_BROADCAST;  // ローカルのネットワークへ．
    }
    socklen_t saddr_len = sizeof(dst_saddr);

    // [debug]
    char buf0[MY_INET_ADDRSTRLEN];
    GetSocketAddress((struct sockaddr *)&dst_saddr, buf0, sizeof(buf0));

    // (4) sendto(): 一定間隔でメッセージをブロードキャストする．
    const char *base_msg = "good morning! Hello everyone!! \\(o v o)/";
    char msg[P_BUF_SIZE];
    for(int i = 0;; ++i) {
        snprintf(msg, P_BUF_SIZE, "%dth %s", i + 1, base_msg);
        size_t len = strlen(msg);
        ssize_t n = sendto(sock, msg, len, 0, (struct sockaddr *)&dst_saddr, saddr_len);
        if(n < len) DieWithSystemMessage(__LINE__, errno, "sendto()");

        // [debug]
        printf("[%d] broadcast to %s\n", i, buf0);
        printf("         message: \"%s\"\n", msg);
        printf("         size:    %ld bytes\n", len);
        fflush(stdout);

        sleep(3);  // ネットワークがいっぱいになるのを防ぐ．
    }

    // (5) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
