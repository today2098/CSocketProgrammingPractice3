#ifndef C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_DIE_WITH_SYSTEM_MESSAGE_H
#define C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_DIE_WITH_SYSTEM_MESSAGE_H 1

// Print system message and terminate program.
void DieWithSystemMessage(int line, const char *msg);

// Print system message with specifying errno and terminate program.
void DieWithSystemMessage2(int line, const char *msg, int no);

#endif
