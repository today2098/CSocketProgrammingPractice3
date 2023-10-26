#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "my_library/get_socket_address.h"
#include "my_library/handle_error.h"
#include "protocol.h"

// TCPコネクションに関するデータを格納する構造体．
struct ClientData {
    int id;
    int sock;
    struct sockaddr_in saddr;
};

// 英文字cの大小を反転させる．
void convert(char *c);
// コネクション確立後に行われるプログラム．
void *thread_func(void *arg);

int main() {
    int ret;

    // (1) socket(): Listen用ソケットを作成．
    int sock0 = socket(PF_INET, SOCK_STREAM, 0);
    if(sock0 == -1) DieWithSystemMessage(__LINE__, "socket()", errno);

    // ソケット名の重複利用に関する制限を緩める（"bind(): Address already in use" のエラー対策）．
    int val = 1;
    setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));

    // (2) bind(): Listen用ソケットに名前付け．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sock0, (struct sockaddr *)&server, sizeof(server));
    if(ret == -1) DieWithSystemMessage(__LINE__, "bind()", errno);

    // (3) listen(): 接続要求を待ち受ける状態にする．
    ret = listen(sock0, 5);
    if(ret == -1) DieWithSystemMessage(__LINE__, "listen()", errno);

    printf("listen now...\n");
    fflush(stdout);

    for(int i = 0;; ++i) {
        // (4) malloc(): 新規TCPコネクションに関する情報を格納する構造体を作成する．
        struct ClientData *cdata = malloc(sizeof(struct ClientData));
        if(cdata == NULL) DieWithSystemMessage(__LINE__, "malloc()", errno);
        cdata->id = i;

        // (5) accept(): TCPクライアントからの接続要求を受け付ける．
        socklen_t len = sizeof(cdata->saddr);
        cdata->sock = accept(sock0, (struct sockaddr *)&cdata->saddr, &len);
        if(cdata->sock == -1) DieWithSystemMessage(__LINE__, "accept()", errno);

        // [debug] クライアントのソケットアドレスを表示．
        char buf0[MY_INET_ADDRSTRLEN];
        GetSocketAddress((struct sockaddr *)&cdata->saddr, buf0, sizeof(buf0));
        printf("[%d] accept form %s\n", i, buf0);
        fflush(stdout);

        // (6) pthread_create(): スレッドを作成し，thread_func()を実行させる．
        pthread_t th;
        ret = pthread_create(&th, NULL, thread_func, cdata);
        if(ret != 0) DieWithSystemMessage(__LINE__, "pthread_create()", ret);

        // (7) pthread_detach(): 親スレッドと子スレッドを切り離す．
        ret = pthread_detach(th);
        if(ret != 0) DieWithSystemMessage(__LINE__, "pthread_detach()", ret);
    }

    // (8) close(): Listen用ソケットを閉じる．
    ret = close(sock0);
    if(ret == -1) DieWithSystemMessage(__LINE__, "close()", errno);
    return 0;
}

void convert(char *c) {
    if('A' <= *c && *c <= 'Z') *c = *c - 'A' + 'a';
    else if('a' <= *c && *c <= 'z') *c = *c - 'a' + 'A';
}

void *thread_func(void *arg) {
    if(arg == NULL) return (void *)-1;

    struct ClientData *cdata = arg;
    int sock = cdata->sock;
    char buf0[MY_INET_ADDRSTRLEN];
    char buf[BUF_SIZE];
    ssize_t n;
    int ret;

    // (a-1) read(): 文字列を受信する．
    n = read(sock, buf, sizeof(buf) - 1);
    if(n == -1) {
        PrintSystemMessage(__LINE__, "read()", errno);
        goto Err;
    }
    buf[n] = '\0';

    // [debug]
    GetSocketAddress((struct sockaddr *)&cdata->saddr, buf0, sizeof(buf0));
    printf("[%d] receive message from %s\n", cdata->id, buf0);
    printf("        message: \"%s\"\n", buf);
    printf("        size:    %ld bytes\n", n);
    fflush(stdout);

    // (a-2) 文字列を変形させる．
    for(ssize_t i = 0; i < n; ++i) convert(&buf[i]);

    // (a-3) write(): 変更した文字列を送信する．
    n = write(sock, buf, strlen(buf));
    if(n == -1) {
        PrintSystemMessage(__LINE__, "write()", errno);
        goto Err;
    }

    // [debug]
    printf("[%d] send converted message to %s\n", cdata->id, buf0);
    printf("        message: \"%s\"\n", buf);
    printf("        size:    %ld bytes\n", n);
    fflush(stdout);

    // (a-4) close(): TCPセッションを終了する．
    ret = close(sock);
    if(ret == -1) {
        PrintSystemMessage(__LINE__, "close()", errno);
        goto Err;
    }

    // (a-5) free(): TCPコネクションに関する情報を格納した構造体のメモリを解放する．
    free(arg);
    return NULL;

Err:
    free(arg);
    return (void *)-1;
}
