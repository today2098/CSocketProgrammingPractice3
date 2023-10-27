#include "handle_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void PrintSystemMessage(int line, int no, const char *context) {
    fprintf(stderr, "[error] line: %d, errno: %d, ", line, no);
    if(context == NULL) fprintf(stderr, "message: %s\n", strerror(no));
    else fprintf(stderr, "%s: %s\n", context, strerror(no));
}

void PrintSystemMessage2(const char *filename, int line, int no, const char *context) {
    fprintf(stderr, "[error] filename: %s, line: %d, errno: %d, ", filename, line, no);
    if(context == NULL) fprintf(stderr, "message: %s\n", strerror(no));
    else fprintf(stderr, "%s: %s\n", context, strerror(no));
}

void DieWithSystemMessage(int line, int no, const char *context) {
    PrintSystemMessage(line, no, context);
    exit(1);
}

void DieWithSystemMessage2(const char *filename, int line, int no, const char *context) {
    PrintSystemMessage2(filename, line, no, context);
    exit(1);
}
