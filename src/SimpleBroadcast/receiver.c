#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/get_socket_address.h"
#include "my_library/handle_error.h"
#include "protocol.h"

int main() {
    int ret;

    // (1) socket(): UDPソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) {
        perror("socket()");
        return 1;
    }

    // (2) bind(): ソケットに名前付け．
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(P_PORT);
    saddr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sock, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind()");
        return 1;
    }

    // (3) recvfrom(): メッセージを受信．
    char buf[P_BUF_SIZE];
    struct sockaddr_in src_saddr;
    socklen_t saddr_len = sizeof(src_saddr);
    char buf0[MY_INET_ADDRSTRLEN];
    for(int i = 0;; ++i) {
        ssize_t n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&src_saddr, &saddr_len);
        if(n < 0) DieWithSystemMessage(__LINE__, errno, "recvfrom()");
        buf[n] = '\0';

        // [debug] メッセージを表示．
        GetSocketAddress((struct sockaddr *)&src_saddr, buf0, sizeof(buf0));
        printf("[%d] receive message from %s\n", i, buf0);
        printf("         message: \"%s\"\n", buf);
        printf("         size:    %ld bytes\n", n);
        fflush(stdout);
    }

    // (4) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
