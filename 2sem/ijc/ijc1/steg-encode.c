// steg-encode.c
// Řešení IJC-DU1, příklad b), 2022-03-04
// Autor: Tomáš Brablec
// Přeloženo: gcc 10.2.1
// enkodér steganografie do PPM pro účely testování

#include "bitset.h"
#include "eratosthenes.c"
#include "error.h"
#include "ppm.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_PRIME 101

// start_prime doesnt have to be a prime
// (message will start at closest prime above)
//
// message must be null terminated string
int main(int argc, char **argv) {
    char *file_in = argv[1];
    char *file_out = argv[2];
    char *message = argv[3];

    if (argc != 4)
        return EXIT_FAILURE;

    struct ppm *image = ppm_read(file_in);
    if (image == NULL) {
        error_exit("Invalid PPM image: %s\n", message);
        goto ERROR_EXIT;
    }

    ul image_size = 3 * image->xsize * image->ysize;

    bitset_alloc(primes, image_size);
    eratosthenes(primes);

    int bit_idx = 0, message_idx = 0;
    bool completed = false;

    for (ul i = START_PRIME; i < image_size; i++) {
        if (bitset_getbit(primes, i) == 0) {
            image->data[i] &= ~(char)1;
            image->data[i] ^= (message[message_idx] >> bit_idx) & 1;

            bit_idx++;

            if (bit_idx == 8) {
                if (message[message_idx] == 0) {
                    completed = true;
                    break;
                }

                bit_idx = 0;
                message_idx++;
            }
        }
    }

    ppm_write(image, file_out);

    bitset_free(primes);
    ppm_free(image);
    if (completed)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;

ERROR_EXIT:
    return EXIT_FAILURE;
}
