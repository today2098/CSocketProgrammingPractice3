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
            "Usage:   %s [port number]\n"
            "Example: %s 12345\n"
            "Broadcast Message (receiver).\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[]) {
    if(!(1 <= argc && argc <= 2)) {
        Usage(argv);
        return 1;
    }

    const char *port_str = (argc == 2 ? argv[1] : P_PORT_STR);
    int ret;

    // (1) socket(): データグラムソケットを作成．
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // (2) bind(): ソケットに名前付け．
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(port_str));
    saddr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sock, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "bind()");

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
        printf("        message: \"%s\"\n", buf);
        printf("        size:    %ld bytes\n", n);
        fflush(stdout);
    }

    // (4) close(): ソケットを閉じる．
    close(sock);
    return 0;
}
