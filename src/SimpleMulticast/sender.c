#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    const char *mcastaddr = "239.192.1.2";
    const char *port_str = "12345";
    int ret;

    // (1) socket(): UDPソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (2) 接続先指定用のアドレス構造体を用意．
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port_str));
    ret = inet_pton(AF_INET, mcastaddr, &addr.sin_addr.s_addr);
    if(ret == 0 || ret == -1) {
        if(ret == 0) fprintf(stderr, "wrong network address notation\n");
        else perror("inet_pton");
        return 1;
    }

    // (3) setsockopt(): 複数のNICがある場合に備えて，出力インターフェイスのIPアドレスを設定する．
    if(argc == 2) {
        in_addr_t interface;
        ret = inet_pton(AF_INET, argv[1], &interface);
        if(ret == 0 || ret == -1) {
            if(ret == 0) fprintf(stderr, "wrong network address notation\n");
            else perror("inet_pton");
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
    ssize_t n = sendto(sock, message, strlen(message), 0, (struct sockaddr *)&addr, sizeof(addr));
    if(n == -1) {
        perror("sendto()");
        return 1;
    }

    // [debug]
    printf("send message to %s:%s\n", mcastaddr, port_str);

    // (5) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
