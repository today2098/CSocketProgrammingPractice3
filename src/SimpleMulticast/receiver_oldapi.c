#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    const char *mcast_addr = "239.192.1.2";
    const char *port_str = "12345";
    int ret;

    // (1) socket(): UDPソケットを作成．
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (2) bind(): ソケットに名前付けする．
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(port_str));
    saddr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sock, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind()");
        return 1;
    }

    // (3) マルチキャストアドレスを設定．
    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_interface.s_addr = INADDR_ANY;  // 任意のインターフェイス．
    ret = inet_pton(AF_INET, mcast_addr, &mreq.imr_multiaddr.s_addr);
    if(ret == -1) {
        perror("inet_pton()");
        return 1;
    }
    if(ret == 0) {
        fprintf(stderr, "wrong network address notation\n");
        return 1;
    }

    // (4) マルチキャストグループにジョインする．
    // 旧APIであるIP_ADD_MEMBERSHIPとip_mreq構造体を利用．
    // IPv6の場合は，別のIPV6_ADD_MEMBERSHIPとipv6_mreq構造体を利用しており，この実装はプロトコル依存である．
    ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if(ret == -1) {
        perror("setsockopt()");
        return 1;
    }

    printf("join to %s:%s\n", mcast_addr, port_str);
    fflush(stdout);

    // (5) recv(): メッセージを受信．
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

    // (6) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
