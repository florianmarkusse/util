#ifndef ERROR_H
#define ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/**
 * @brief Print an error message to the standard error stream (stderr).
 *
 * This macro allows you to print an error message to the standard error stream
 * (stderr). It takes a variable number of arguments, similar to the `printf`
 * function, and prints the error message to stderr.
 *
 * @param[in]   ...     The error message format and arguments.
 */
#define FLO_PRINT_ERROR(...) fprintf(stderr, __VA_ARGS__)

/**
 * @brief Print an error message with a code and message.
 *
 * This macro prints an error message along with an error code to the standard
 * error stream (stderr). It takes an error code and a message as input and
 * prints them to stderr.
 *
 * @param[in]   code        The error code to print.
 * @param[in]   message     The error message to print.
 */
#define FLO_ERROR_WITH_CODE_ONLY(code, message)                                \
    do {                                                                       \
        FLO_PRINT_ERROR("Error code: %s\n", code);                             \
        FLO_PRINT_ERROR("%s\n\n", message);                                    \
    } while (0)

/**
 * @brief Print an error message with a code and formatted message.
 *
 * This macro prints an error message along with an error code to the standard
 * error stream (stderr). It takes an error code and a formatted message as
 * input and prints them to stderr.
 *
 * @param[in]   code        The error code to print.
 * @param[in]   format      The format string for the error message.
 * @param[in]   ...         The arguments to format the error message.
 */
#define FLO_ERROR_WITH_CODE_FORMAT(code, format, ...)                          \
    do {                                                                       \
        FLO_PRINT_ERROR("Error code: %s\n", code);                             \
        FLO_PRINT_ERROR(format "\n\n", __VA_ARGS__);                           \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif
