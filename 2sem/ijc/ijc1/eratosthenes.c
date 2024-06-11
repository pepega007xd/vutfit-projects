// eratosthenes.c
// Řešení IJC-DU1, příklad a), 2022-03-04
// Autor: Tomáš Brablec
// Přeloženo: gcc 10.2.1

#ifndef ERATOSTHENES_C
#define ERATOSTHENES_C

#include "bitset.h"
#include <math.h>
#include <stdio.h>

void eratosthenes(bitset_t bitset) {
    ul size = bitset_size(bitset);

    bitset_setbit(bitset, 0, 1);
    bitset_setbit(bitset, 1, 1);

    for (bitset_index_t i = 2; i < ceil(sqrt((double)size)); i++) {
        if (bitset_getbit(bitset, i)) {
            continue;
        }

        for (bitset_index_t j = 2 * i; j < size; j += i) {
            bitset_setbit(bitset, j, 1);
        }
    }
}

#endif // ERATOSTHENES_C
