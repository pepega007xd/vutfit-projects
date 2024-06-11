#ifndef IMPORTS_H
#define IMPORTS_H

/// this header imports allocator wrapper, so it is included in all files in the
/// project, also all standard library headers are imported here, to avoid
/// polluting source files with many import lines
///
/// this header also renames exact integer types to have shorter names, these
/// are used in the rest of the project instead of the usual types like "int" to
/// avoid uncertainty in how big the types actually are
///
/// some useful debugging macros are also defined here

// import allocator before anything else
#include "sgc.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

// better integer types

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// debugging macros

/// print "HERE: " with optional formatted message
#define here(...)                                                              \
    {                                                                          \
        fprintf(stderr, "HERE: ");                                             \
        fprintf(stderr, __VA_ARGS__);                                          \
        fprintf(stderr, "\n");                                                 \
    }

/// crash program and print stacktrace (works only with -fsanitize=address)
#define trace() *(char *)0 = 0;

#endif // !IMPORTS_H
