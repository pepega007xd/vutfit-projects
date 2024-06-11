/*
 * buffer.h
 *
 * xhashm00 Abas Hashmy
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef BUFFER_H
#define BUFFER_H

#include "global_include.h"

/// dynamically resized data buffer (stores an array of chars),
/// can be used as a string (is null-terminated)
typedef struct {
        char *content;
        unsigned index;
        size_t capacity;
        size_t length;
        unsigned line;   /// Number of the current line.
        unsigned column; /// Number of the current character on a line.
} Buffer;

/// returns next character in buffer without consuming it
/// if the next symbol is EOF returns NULL byte
char peek_next_char(Buffer *buffer);

/// Consumes character in the buffer and returns the next one.
/// If the next symbol is EOF, returns NULL byte.
/// Increments character variable and if it finds newline,
/// it resets character variable and increments
/// line variable.
char consume_next_char(Buffer *buffer);

/// Returns the current character without consuming it
char peek_char(Buffer *buffer);

/// creates buffer object and allocates memeory
Buffer buffer_create();

/// pushes single character on the end of buffer,
void buffer_push(Buffer *buffer, char c);

/// appends a null-terminated string to buffer
/// returns number of pushed bytes
size_t buffer_append(Buffer *buffer, char *str);

/// deletes all data from buffer
void buffer_clear(Buffer *buffer);

/// prints buffer content to `output`
void buffer_print(Buffer *buffer, FILE *output);

/// deallocates memory owned by buffer
void buffer_destroy(Buffer *buffer);

#endif // !BUFFER_H
