// steg-decode.c
// Řešení IJC-DU1, příklad b), 2022-03-04
// Autor: Tomáš Brablec
// Přeloženo: gcc 10.2.1

#include "bitset.h"
#include "eratosthenes.c"
#include "error.h"
#include "ppm.h"
#include "utf8_check.c"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define START_PRIME 101

int main(int argc, char **argv) {
    if (argc == 1 || argc > 2) {
        warning("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct ppm *image = ppm_read(argv[1]);
    if (image == NULL) {
        error_exit("Invalid PPM image: %s\n", argv[1]);
        goto ERROR_EXIT;
    }

    ul image_size = 3 * image->xsize * image->ysize;

    bitset_alloc(primes, image_size);
    eratosthenes(primes);

    ul max_message_size = 0;
    for (ul i = START_PRIME; i < bitset_size(primes); i++) {
        if (bitset_getbit(primes, i) == 0) {
            max_message_size++;
        }
    }

    char *message = calloc(max_message_size, 1);
    if (message == NULL) {
        warning("Allocation failed\n");
        goto ERROR_DEALLOC_BITSET;
    }

    int bit_idx = 0, message_idx = 0;

    bool terminated = false;
    for (ul i = START_PRIME; i < image_size; i++) {
        if (bitset_getbit(primes, i) == 0) {
            message[message_idx] ^= (image->data[i] & 1) << bit_idx;

            bit_idx++;

            if (bit_idx == 8) {
                if (message[message_idx] == 0) {
                    terminated = true;
                    break;
                }

                bit_idx = 0;
                message_idx++;
            }
        }
    }

    if (!terminated) {
        warning("Message is not null-terminated\n");
        goto ERROR_DEALLOC_MESSAGE;
    }

    if (utf8_check((unsigned char *)message) != NULL) {
        warning("Invalid UTF-8 character in message\n");
        // printf("%s\n", message);
        goto ERROR_DEALLOC_MESSAGE;
    }

    printf("%s\n", message);

    free(message);
    bitset_free(primes);
    ppm_free(image);
    return EXIT_SUCCESS;

ERROR_DEALLOC_MESSAGE:
    free(message);

ERROR_DEALLOC_BITSET:
    bitset_free(primes);
    ppm_free(image);

ERROR_EXIT:
    return EXIT_FAILURE;
}
