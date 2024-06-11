// primes.c
// Řešení IJC-DU1, příklad a), 2022-03-04
// Autor: Tomáš Brablec
// Přeloženo: gcc 10.2.1

#include "bitset.h"
#include "eratosthenes.c"
#include <stdio.h>
#include <time.h>

#define N 230000000

void print_bitset(bitset_t bitset) {
    for (ul i = 0; i < bitset_size(bitset); i++) {
        printf("%d", bitset_getbit(bitset, i));
    }

    printf("\n");
}

int main(void) {
    bitset_create(bitset, N);

    clock_t start = clock();
    eratosthenes(bitset);
    fprintf(stderr, "Time=%.3g\n", (double)(clock() - start) / CLOCKS_PER_SEC);

    ul primes[10];
    bitset_index_t idx = N - 1;

    for (int i = 9; i >= 0;) {
        if (!bitset_getbit(bitset, idx)) {
            primes[i] = idx;
            i--;
        }
        idx--;
    }

    for (int i = 0; i < 10; i++) {
        printf("%lu\n", primes[i]);
    }
}
