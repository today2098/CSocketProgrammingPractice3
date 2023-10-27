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
#include "my_library/presentation_layer.h"
#include "protocol.h"

struct ThreadArgs {
    struct sockaddr_in saddr;
    int id;
    int sock;
};

// 英文字cの大小を変更する．
void convert(char *c);

void *thread_main(void *arg);

int main() {
    int ret;

    // (1) socket(): Listen用ソケットを作成．
    int sock0 = socket(PF_INET, SOCK_STREAM, 0);
    if(sock0 == -1) DieWithSystemMessage(__LINE__, errno, "socket()");

    // setsockopt(): ソケット名の重複利用に関する制限を緩める．
    // "bind(): Address already in use" のエラー対策．
    int val = 1;
    ret = setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "setsockopt()");

    // (2) bind(): Listen用ソケットに名前付け．
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sock0, (struct sockaddr *)&server, sizeof(server));
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "bind()");

    // (3) listen(): 接続要求の受け付けを開始．
    ret = listen(sock0, 5);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "listen()");

    printf("listen now...\n");
    fflush(stdout);

    for(int i = 0;; ++i) {
        // malloc(): 新しいスレッドに渡す引数を用意．
        struct ThreadArgs *argdata = malloc(sizeof(struct ThreadArgs));
        if(argdata == NULL) DieWithSystemMessage(__LINE__, errno, "malloc()");
        argdata->id = i;

        // (4) accept(): TCPクライアントからの接続要求を受け付ける．
        socklen_t len = sizeof(argdata->saddr);
        argdata->sock = accept(sock0, (struct sockaddr *)&argdata->saddr, &len);
        if(argdata->sock == -1) DieWithSystemMessage(__LINE__, errno, "accept()");

        // [debug] クライアントのソケットアドレスを表示．
        char buf0[MY_INET_ADDRSTRLEN];
        GetSocketAddress((struct sockaddr *)&argdata->saddr, buf0, sizeof(buf0));
        printf("[%d] accept form %s\n", i, buf0);
        fflush(stdout);

        // (5) pthread_create(): スレッドを作成．
        pthread_t th;
        ret = pthread_create(&th, NULL, thread_main, argdata);
        if(ret != 0) DieWithSystemMessage(__LINE__, ret, "pthread_create()");

        // (6) pthread_detach(): 親スレッドと子スレッドを切り離す．
        ret = pthread_detach(th);
        if(ret != 0) DieWithSystemMessage(__LINE__, ret, "pthread_detach()");
    }

    // (7) close(): Listen用ソケットを閉じる．
    ret = close(sock0);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "close()");
    return 0;
}

void convert(char *c) {
    if('A' <= *c && *c <= 'Z') *c = *c - 'A' + 'a';
    else if('a' <= *c && *c <= 'z') *c = *c - 'a' + 'A';
}

void *thread_main(void *arg) {
    if(arg == NULL) return (void *)-1;

    struct ThreadArgs *argdata = (struct ThreadArgs *)arg;
    int id = argdata->id;
    int sock = argdata->sock;
    char buf0[MY_INET_ADDRSTRLEN];
    char buf[BUF_SIZE];
    size_t len;
    int ret;

    // (a-1) read(): 文字列を受信．
    ret = Read64bits(sock, &len);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "read()");
    ret = ReadByteString(sock, buf, len);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "read()");
    buf[len] = '\0';

    // [debug]
    GetSocketAddress((struct sockaddr *)&argdata->saddr, buf0, sizeof(buf0));
    printf("[%d] receive message from %s\n", id, buf0);
    printf("       message: \"%s\"\n", buf);
    printf("       size:    %ld bytes\n", len);
    fflush(stdout);

    // (a-2) 文字列を変形させる．
    for(size_t i = 0; i < len; ++i) convert(&buf[i]);

    // (a-3) write(): 変更した文字列を送信．
    len = strlen(buf);
    ret = Write64bits(sock, len);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "write()");
    ret = WriteByteString(sock, buf, len);
    if(ret == -1) DieWithSystemMessage(__LINE__, errno, "write()");

    // [debug]
    printf("[%d] send converted message to %s\n", id, buf0);
    printf("       message: \"%s\"\n", buf);
    printf("       size:    %ld bytes\n", len);
    fflush(stdout);

    // (a-4) close(): TCPセッションを終了．
    ret = close(sock);
    if(ret == -1) {
        PrintSystemMessage(__LINE__, errno, "close()");
        goto Err;
    }

    // (a-5) free(): TCPコネクションに関する情報を格納した構造体のメモリを解放する．
    free(arg);
    return NULL;

Err:
    free(arg);
    return (void *)-1;
}
