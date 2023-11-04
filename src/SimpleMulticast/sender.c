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
            "Usage:   %s [multicast IP address] [port number] [output interface]\n"
            "Example: %s 239.192.1.2 12345 192.168.1.3\n"
            "Message Transfer by Multicast (sender).\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
    if(!(1 <= argc && argc <= 4)) {
        Usage(argv);
        return 1;
    }

    const char *mcast_ipaddr_str = (argc >= 2 ? argv[1] : P_MULTICAST_IPADDR);
    const uint16_t port = (argc >= 3 ? atoi(argv[2]) : P_PORT);
    const char *output_ipaddr_str = (argc == 4 ? argv[3] : NULL);
    int ret;

    // (1) socket(): データグラムソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // (2) 宛先指定用のアドレス構造体を用意．
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    ret = inet_pton(AF_INET, mcast_ipaddr_str, &saddr.sin_addr.s_addr);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "inet_ntop()");
    if(ret == 0) {
        fprintf(stderr, "[error] line: %d, inet_ntop(): wrong network address notation\n", __LINE__);
        return 1;
    }

    // (3) setsockopt(): NICが複数ある場合に備えて，出力インターフェイスのIPアドレスを設定する．
    if(argc == 4) {
        struct in_addr interface;
        ret = inet_pton(AF_INET, output_ipaddr_str, &interface.s_addr);
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "inet_ntop()");
        if(ret == 0) {
            fprintf(stderr, "[error] line: %d, inet_pton(): wrong network address notation\n", __LINE__);
            return 1;
        }
        ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &interface, sizeof(interface));
        if(ret == -1) DieWithSystemMessage(__LINE__, errno, "setsockopt()");
    }

    // (4) setsockopt(): TTLを設定する．
    int ttl = 1;  // デフォルトは1．
    ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "setsockopt()");

    // (5) sendto(): 一定間隔でメッセージをマルチキャストする．
    const char *base_msg = "good morning! Hello everyone!! \\(0 v 0)/";
    char msg[P_BUF_SIZE];
    for(int i = 0;; ++i) {
        snprintf(msg, sizeof(msg), "%dth %s", i + 1, base_msg);
        ssize_t n = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&saddr, sizeof(saddr));
        if(n < strlen(msg)) DieWithSystemMessage(__LINE__, errno, "sendto()");

        // [debug]
        printf("[%d] multicast to %s:%d\n", i, mcast_ipaddr_str, port);
        printf("        message: \"%s\"\n", msg);
        printf("        size:    %ld bytes\n", strlen(msg));
        fflush(stdout);

        sleep(3);  // ネットワークがいっぱいになるのを防ぐ．
    }

    // (6) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
