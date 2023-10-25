#ifndef C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_HANDLE_ERROR_H
#define C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_HANDLE_ERROR_H 1

// Print system message with specifying errno.
void PrintSystemMessage(int line, const char *context, int no);

// Print system message with specifying errno and terminate program.
void DieWithSystemMessage(int line, const char *context, int no);

#endif
