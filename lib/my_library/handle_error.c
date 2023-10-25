#include "handle_error.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void PrintSystemMessage(int line, const char *context, int no) {
    fprintf(stderr, "[error] line: %d, errno: %d, ", line, no);
    if(context == NULL) fprintf(stderr, "message: %s\n", strerror(no));
    else fprintf(stderr, "%s: %s\n", context, strerror(no));
}

void DieWithSystemMessage(int line, const char *context, int no) {
    PrintSystemMessage(line, context, no);
    exit(1);
}
