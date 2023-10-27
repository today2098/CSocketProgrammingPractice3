#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void Usage(char *argv[]) {
    fprintf(stderr,
            "Usage:   %s <IP address of output interface>\n"
            "Example: %s 192.168.11.3\n"
            "Message Transfer by UDP\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
    if(!(1 <= argc && argc <= 2)) {
        Usage(argv);
        return 1;
    }

    const char *mcast_addr = "239.192.1.2";
    const char *port_str = "12345";
    const char *output_ipaddr = (argc == 2 ? argv[1] : NULL);
    int ret;

    // (1) socket(): UDPソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (2) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in dst_saddr;
    memset(&dst_saddr, 0, sizeof(dst_saddr));
    dst_saddr.sin_family = AF_INET;
    dst_saddr.sin_port = htons(atoi(port_str));
    ret = inet_pton(AF_INET, mcast_addr, &dst_saddr.sin_addr.s_addr);
    if(ret == -1) {
        perror("inet_pton()");
        return 1;
    }
    if(ret == 0) {
        fprintf(stderr, "wrong network address notation\n");
        return 1;
    }

    // (3) setsockopt(): NICが複数ある場合に備えて，出力インターフェイスのIPアドレスを設定する．
    if(argc == 2) {
        in_addr_t interface;
        ret = inet_pton(AF_INET, output_ipaddr, &interface);
        if(ret == -1) {
            perror("inet_pton()");
            return 1;
        }
        if(ret == 0) {
            fprintf(stderr, "wrong network address notation\n");
            return 1;
        }
        ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &interface, sizeof(interface));
        if(ret == -1) {
            perror("setsockopt()");
            return 1;
        }
    }

    // (4) sendto(): メッセージを送信．
    char message[1024] = "Hello, world.";
    ssize_t n = sendto(sock, message, strlen(message), 0, (struct sockaddr *)&dst_saddr, sizeof(dst_saddr));
    if(n == -1) {
        perror("sendto()");
        return 1;
    }

    // [debug]
    printf("send message to %s:%s\n", mcast_addr, port_str);
    fflush(stdout);

    // (5) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
