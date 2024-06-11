// Filename: io.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#include <ctype.h>
#include <stdio.h>
#include <string.h>

// Writes a single word from stream `file` into `str`,
// null-terminated. Writes at most `max - 1` bytes.
// Word is a sequence of characters delimited by `isspace` characters.
int read_word(char *str, int max, FILE *file) {
    int word_len = 0;
    int c;
    memset(str, 0, max);

    // removing any `isspace` chars at the beginning
    while ((c = fgetc(file)) != EOF) {
        if (!isspace(c)) {
            str[word_len] = c;
            word_len++;
            break;
        }
    }

    while ((c = fgetc(file)) != EOF) {
        if (isspace(c))
            break;

        if (word_len < max - 1)
            str[word_len] = c;

        word_len++;
    }

    if (word_len == 0)
        return EOF;

    return word_len;
}
