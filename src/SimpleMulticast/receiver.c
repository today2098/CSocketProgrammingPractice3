#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
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
    const char *port_str = (argc == 3 ? argv[2] : P_PORT_STR);
    struct addrinfo hints, *result0;
    int status;
    char buf0[MY_INET6_ADDRSTRLEN];
    int ret;

    // (1) 名前解決を行う．
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    status = getaddrinfo(NULL, port_str, &hints, &result0);
    if(status != 0) {
        fprintf(stderr, "[error] line: %d, getaddrinfo(): %s\n", __LINE__, gai_strerror(status));
        return 1;
    }

    // (2) socket(): ソケットを作成．
    int sock = socket(result0->ai_family, result0->ai_socktype, result0->ai_protocol);
    if(sock == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // (3) bind(): ソケットに名前付け．
    ret = bind(sock, result0->ai_addr, result0->ai_addrlen);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "bind()");
    freeaddrinfo(result0);

    // (4) マルチキャストアドレスを設定．
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_NUMERICHOST;
    status = getaddrinfo(mcast_ipaddr_str, NULL, &hints, &result0);
    if(status != 0) {
        fprintf(stderr, "[error] line: %d, getaddrinfo(): %s\n", __LINE__, gai_strerror(status));
        return 1;
    }

    // (5) マルチキャストグループにジョインする．
    struct group_req greq;
    memset(&greq, 0, sizeof(greq));
    greq.gr_interface = 0;  // 任意のインターフェイスを利用．
    memcpy(&greq.gr_group, result0->ai_addr, result0->ai_addrlen);
    freeaddrinfo(result0);
    ret = setsockopt(sock, IPPROTO_IP, MCAST_JOIN_GROUP, &greq, sizeof(greq));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "setsockopt()");

    printf("join multicast group %s:%s\n", mcast_ipaddr_str, port_str);
    fflush(stdout);

    // (6) recv(): メッセージを受信．
    char buf[1024];
    struct sockaddr_in peer_saddr;
    socklen_t saddr_len = sizeof(peer_saddr);
    ssize_t n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&peer_saddr, &saddr_len);
    if(n == -1) DieWithSystemMessage(__LINE__, errno, "recvfrom()");
    buf[n] = '\0';

    // [debug] 送信元のソケットアドレスとメッセージを表示．
    // char buf0[MY_INET_ADDRSTRLEN];
    GetSocketAddress((struct sockaddr *)&peer_saddr, buf0, sizeof(buf0));
    printf("receive message from %s\n", buf0);
    printf("    message: %s\n", buf);
    printf("    size:    %ld bytes\n", n);

    // (7) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
