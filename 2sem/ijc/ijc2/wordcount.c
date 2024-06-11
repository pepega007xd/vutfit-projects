// Filename: wordcount.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28
//
// Compiler: gcc-10.2.1-6

#include "htab.h"
#include "io.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define WORD_BUFFER_LEN 256
#define HASH_TABLE_SIZE 1024

#ifdef HASHTEST

// the hash table should work fine, even
// with a horrible hash function
size_t htab_hash_function(htab_key_t str) { return str[0]; }

#endif // HASHTEST

void print_word_count(htab_pair_t *pair) {
    printf("%s\t%d\n", pair->key, pair->value);
}

int main(void) {
    bool size_warning_sent = false;
    htab_t *htab = htab_init(HASH_TABLE_SIZE);
    if (htab == NULL)
        goto ERR_EXIT;

    char word_buf[WORD_BUFFER_LEN];
    int word_size;

    while ((word_size = read_word(word_buf, WORD_BUFFER_LEN, stdin)) != EOF) {
        if (word_size >= WORD_BUFFER_LEN && !size_warning_sent) {
            fprintf(stderr, "WARNING: Maximum word length (%d) exceeded.\n",
                    WORD_BUFFER_LEN - 1);
            size_warning_sent = true;
        }

        htab_pair_t *pair = htab_lookup_add(htab, word_buf);
        if (pair == NULL)
            goto ERR_FREE_HTAB;
        pair->value++;
    }

    htab_for_each(htab, print_word_count);

    htab_free(htab);
    return EXIT_SUCCESS;

ERR_FREE_HTAB:
    htab_free(htab);

ERR_EXIT:
    return EXIT_FAILURE;
}
