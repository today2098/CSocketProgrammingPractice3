#include "handle_error.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void DieWithSystemMessage(int line, const char *msg) {
    fprintf(stderr, "line: %d\n", line);
    perror(msg);
    fprintf(stderr, "errno: %d\n", errno);
    exit(1);
}

void DieWithSystemMessage2(int line, const char *msg, int no) {
    fprintf(stderr, "line: %d\n", line);
    fprintf(stderr, "%s: %s\n", msg, strerror(no));
    fprintf(stderr, "errno: %d\n", no);
    exit(1);
}
