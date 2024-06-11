/*
 * global_include.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "global_include.h"
#include <stdio.h>

/// prints input to `out`, but only up to first newline,
/// including the newline character
void print_line(FILE *out, char *start) {
    if (start == NULL) {
        puts("<cannot print source>");
        return;
    }

    char c = *start++;
    while (c != '\0') {
        putc(c, out);
        if (c == '\n') {
            return;
        }
        c = *start++;
    }

    if (c != '\n') {
        putc('\n', out);
    }
}

void repeat(char c, unsigned times) {
    for (unsigned i = 0; i < times; i++) {
        putc(c, stderr);
    }
}

/// error function
_Noreturn void error_print_exit(Token *token, ErrorType type, char *err_msg) {
    // the error should look like this:
    /*
    Error on line 42:

    let abc = previous ?? default
                          ^^^^^^^
    Wrong type of operand.
     */

    fprintf(stderr, "Error on line %d:\n",
            token->line_number + 1); // index lines from 1
    fprintf(stderr, "\n");

    print_line(stderr, token->line_start);

    repeat(' ', token->char_column);
    repeat('^', token->token_length);
    putc('\n', stderr);

    fprintf(stderr, "%s\n", err_msg);

    sgc_free_all();
    exit(type); // enum values correspond to return codes
}
