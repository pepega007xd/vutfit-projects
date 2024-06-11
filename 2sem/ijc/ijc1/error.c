// error.c
// Řešení IJC-DU1, příklad b), 2022-03-04
// Autor: Tomáš Brablec
// Přeloženo: gcc 10.2.1

#include "error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void warning(const char *fmt, ...) {
    fputs("Warning: ", stderr);

    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);

    va_end(args);
}

void error_exit(const char *fmt, ...) {
    fputs("Error: ", stderr);

    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);

    va_end(args);
    exit(1);
}
