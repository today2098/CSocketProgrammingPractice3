#ifndef C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_HANDLE_ERROR_H
#define C_SOCKET_PROGRAMMING_PRACTICE_MY_LIBRARY_HANDLE_ERROR_H 1

#include <errno.h>

/**
 * @brief Print system message with specifying errno.
 *
 * @param line '__LINE__'
 * @param no 'errno' which is defined in <errno.h>
 * @param context Name of function in which error was detected
 */
void PrintSystemMessage(int line, int no, const char *context);

/**
 * @brief Print system message with specifying errno.
 *
 * @param filename '__FILE__'
 * @param line '__LINE__'
 * @param no 'errno' which is defined in <errno.h>
 * @param context Name of function in which error was detected
 */
void PrintSystemMessage2(const char *filename, int line, int no, const char *context);

/**
 * @brief Print system message with specifying errno and terminate program.
 *
 * @param line '__LINE__'
 * @param no 'errno' which is defined in <errno.h>
 * @param context Name of function in which error was detected
 */
void DieWithSystemMessage(int line, int no, const char *context);

/**
 * @brief Print system message with specifying errno and terminate program.
 *
 * @param filename '__FILE__'
 * @param line '__LINE__'
 * @param no 'errno' which is defined in <errno.h>
 * @param context Name of function in which error was detected
 */
void DieWithSystemMessage2(const char *filename, int line, int no, const char *context);

#endif
