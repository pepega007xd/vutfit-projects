#ifndef UTILS_H
#define UTILS_H

#include "imports.h"

#define MAX_MESSAGE_SIZE 1500

/// Print formatted error message, add newline,
/// free all resources, and exit with failure exit code
noreturn void print_exit(const char *format, ...);

/// Free all resources and exit with failure exit code
noreturn void exit_fail(void);

/// Returns true if true null-terminated strings are the same
bool string_eq(const char *lhs, const char *rhs);

/// Same as `string_eq`, but ascii letters are compared case-insensitively
bool string_eq_nocase(const char *lhs, const char *rhs);

/// converts 16 bit unsigned number from two bytes in Network byte order into
/// local byte order
u16 from_net_bytes(u8 byte0, u8 byte1);

/// Prints formatted message to stderr, appends newline
void eprintf(const char *format, ...);

#endif // !UTILS_H
