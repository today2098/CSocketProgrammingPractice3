#include <arpa/inet.h>
#include <netdb.h>
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
            "Usage:   %s [multicast IP address] [port number]\n"
            "Example: %s 239.192.1.2 12345\n"
            "Message Transfer by Multicast (receiver).\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
    if(!(1 <= argc && argc <= 3)) {
        Usage(argv);
        return 1;
    }

    const char *mcast_ipaddr_str = (argc >= 2 ? argv[1] : P_MULTICAST_IPADDR);
    const uint16_t port = (argc == 3 ? atoi(argv[2]) : P_PORT);
    int ret;

    // (1) socket(): データグラムソケットを作成．
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // (2) bind(): ソケットに名前付け．
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sock, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "bind()");

    // (3) マルチキャストアドレスを設定．
    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_interface.s_addr = INADDR_ANY;  // 任意のインターフェイスを利用．
    ret = inet_pton(AF_INET, mcast_ipaddr_str, &mreq.imr_multiaddr.s_addr);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "inet_ntop()");
    if(ret == 0) {
        fprintf(stderr, "[error] line: %d, inet_ntop(): wrong network address notation\n", __LINE__);
        return 1;
    }

    // (4) setsockopt(): マルチキャストグループにジョインする．
    // IPv4の場合，旧APIであるIP_ADD_MEMBERSHIPオプションとip_mreq構造体を利用．
    // IPv6の場合は，別のIPV6_ADD_MEMBERSHIPとipv6_mreq構造体を利用しており，この実装はプロトコル依存である．
    ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "setsockopt()");

    printf("join multicast group %s:%d\n", mcast_ipaddr_str, port);
    fflush(stdout);

    // (5) recvfrom(): メッセージを受信．
    char buf[1024];
    struct sockaddr_in peer_saddr;
    socklen_t saddr_len = sizeof(peer_saddr);
    ssize_t n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&peer_saddr, &saddr_len);
    if(n == -1) DieWithSystemMessage(__LINE__, errno, "recvfrom()");
    buf[n] = '\0';

    // [debug] 送信元のソケットアドレスとメッセージを表示．
    char buf0[MY_INET_ADDRSTRLEN];
    GetSocketAddress((struct sockaddr *)&peer_saddr, buf0, sizeof(buf0));
    printf("receive message from %s\n", buf0);
    printf("    message: %s\n", buf);
    printf("    size:    %ld bytes\n", n);

    // (6) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
