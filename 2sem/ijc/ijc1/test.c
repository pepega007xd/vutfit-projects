// test.c
// Řešení IJC-DU1, testovaci program, 2022-03-04
// Autor: Tomáš Brablec
// Přeloženo: gcc 10.2.1
// spouští se uvnitř test.sh

#include "bitset.h"
#include <stdio.h>

#define TESTSIZE 11949
#define RANDPOINT 8733

void do_bitset_test(bitset_t bitset) {
    assert(bitset_size(bitset) == TESTSIZE);

    assert(bitset_getbit(bitset, RANDPOINT) == 0);
    assert(bitset_getbit(bitset, 0) == 0);
    assert(bitset_getbit(bitset, TESTSIZE - 1) == 0);

    bitset_setbit(bitset, RANDPOINT, 1);
    bitset_setbit(bitset, TESTSIZE - 1, 1);
    bitset_setbit(bitset, 0, 1);

    assert(bitset_getbit(bitset, 0) == 1);
    assert(bitset_getbit(bitset, TESTSIZE - 1) == 1);

    assert(bitset_getbit(bitset, RANDPOINT - 1) == 0);
    assert(bitset_getbit(bitset, RANDPOINT) == 1);
    assert(bitset_getbit(bitset, RANDPOINT + 1) == 0);
}

void bitset_test(void) {
    bitset_create(locfield, TESTSIZE);
    static bitset_create(statfield, TESTSIZE);
    bitset_alloc(dynfield, TESTSIZE);

    do_bitset_test(locfield);
    do_bitset_test(statfield);
    do_bitset_test(dynfield);

    bitset_free(dynfield);
}

int main() {
    bitset_test();

    puts("all tests finished");
    return (0);
}
