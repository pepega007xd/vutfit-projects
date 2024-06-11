/*
 * buffer.c
 *
 * xhashm00 Abas Hashmy
 * xbrabl04 Tomáš Brablec
 *
 */

#include "buffer.h"
#include "global_include.h"

Buffer buffer_create() {
    Buffer buf;
    buf.content = sgc_malloc(1);
    buf.content[0] = '\0'; // the buffer is null-terminated
    buf.capacity = 1;
    buf.length = 0; // null-terminating byte doesn't count to length
    buf.line = 0;
    buf.column = 0;
    buf.index = 0;
    return buf;
}

void buffer_push(Buffer *buffer, char c) {
    if (buffer->length + 1 == buffer->capacity) {
        buffer->content = sgc_realloc(buffer->content, buffer->capacity * 2);
        buffer->capacity *= 2;
    }

    buffer->content[buffer->length++] = c;
    buffer->content[buffer->length] = '\0';
}

size_t buffer_append(Buffer *buffer, char *str) {
    size_t append_len = strlen(str);
    size_t new_length = buffer->length + 1 + append_len;

    if (new_length >= buffer->capacity) {
        // to prevent frequent reallocations when appending many short strings
        buffer->content = sgc_realloc(buffer->content, new_length * 2);
        buffer->capacity = new_length * 2;
    }

    strcpy(&buffer->content[buffer->length], str);
    buffer->length += append_len;
    buffer->content[buffer->length] = '\0';

    return append_len;
}

void buffer_clear(Buffer *buffer) {
    buffer->length = 0;
    buffer->content[0] = '\0';
}

void buffer_print(Buffer *buffer, FILE *output) {
    fprintf(output, "%*s\n", (int)buffer->length, buffer->content);
}

void buffer_destroy(Buffer *buffer) {
    sgc_free(buffer->content);
    buffer->content = NULL;
    buffer->length = 0;
    buffer->capacity = 0;
}

char peek_next_char(Buffer *buffer) {
    return buffer->content[buffer->index + 1];
}

char consume_next_char(Buffer *buffer) {
    // check for end of buffer
    if (buffer->content[buffer->index] == 0) {
        return '\0';
    }

    if (buffer->content[buffer->index] == '\n') {
        buffer->line++;
        buffer->column = 0;
    } else {
        buffer->column++;
    }

    return buffer->content[++buffer->index];
}

char peek_char(Buffer *buffer) { return buffer->content[buffer->index]; }
