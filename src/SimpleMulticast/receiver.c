#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    const char *mcast_addr = "239.192.1.2";
    const char *port_str = "12345";
    struct addrinfo hints, *result0;
    int status;
    int ret;

    // (1) 名前解決処理を行う．
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    status = getaddrinfo(NULL, port_str, &hints, &result0);
    if(status != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
        return 1;
    }

    // (2) socket(): ソケットを作成．
    int sock = socket(result0->ai_family, result0->ai_socktype, result0->ai_protocol);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (3) bind(): ソケットに名前付け．
    ret = bind(sock, result0->ai_addr, result0->ai_addrlen);
    if(ret == -1) {
        perror("bind()");
        return 1;
    }
    freeaddrinfo(result0);

    // (4) マルチキャストアドレスを設定．
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    status = getaddrinfo(mcast_addr, NULL, &hints, &result0);
    if(status != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
        return 1;
    }

    // (5) マルチキャストグループにジョインする．
    struct group_req greq;
    memset(&greq, 0, sizeof(greq));
    greq.gr_interface = 0;  // 任意のインターフェイスを利用．
    memcpy(&greq.gr_group, result0->ai_addr, result0->ai_addrlen);
    freeaddrinfo(result0);
    ret = setsockopt(sock, IPPROTO_IP, MCAST_JOIN_GROUP, &greq, sizeof(greq));
    if(ret == -1) {
        perror("setsockopt()");
        return 1;
    }

    printf("join to %s:%s\n", mcast_addr, port_str);
    fflush(stdout);

    // (6) recv(): メッセージを受信．
    char buf[1024];
    ssize_t n = read(sock, buf, sizeof(buf) - 1);
    if(n == -1) {
        perror("read()");
        return 1;
    }
    buf[n] = '\0';

    // [debug] メッセージを表示．
    printf("receive message\n");
    printf("  message: %s\n", buf);
    printf("  size: %ld bytes\n", n);

    // (7) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
