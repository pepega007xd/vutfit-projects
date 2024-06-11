#ifndef UTILS_H
#define UTILS_H

#include "imports.h"

#define BUFFER_SIZE 1500

/// Print formatted error message, add newline,
/// free all resources, and exit
noreturn void print_exit(const char *format, ...);

/// deallocates all memory and terminates the program
noreturn void exit_fail(void);

/// Prints formatted message to stderr, appends newline
void eprintf(const char *format, ...);

/// returns true if the two strings are equal
bool string_eq(const char *lhs, const char *rhs);

/// returns true if the two strings are equal,
/// ascii letters are compared case-insensitively
bool string_eq_nocase(const char *lhs, const char *rhs);

/// returns true if `string` is an empty string
bool string_is_empty(const char *string);

/// get current time in milliseconds
i64 get_time_ms(void);

#endif // !UTILS_H
