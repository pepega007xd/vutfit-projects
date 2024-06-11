// bitset.h
// Řešení IJC-DU1, příklad a), 2022-03-04
// Autor: Tomáš Brablec
// Přeloženo: gcc 10.2.1

#ifndef BITSET_MODULE
#define BITSET_MODULE

#include "error.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long ul;
typedef ul *bitset_t;
typedef ul bitset_index_t;
typedef long long ll;

#define _arr_idx(idx) ((idx) / sizeof(ul) / 8 + 1)
#define _bit_idx(idx) ((idx) % (sizeof(ul) * 8))
#define _bitset_size(size) ((size) / sizeof(ul) / 8 + 2)

#define bitset_create(name, size)                                              \
    ul name[_bitset_size(size)];                                               \
    {                                                                          \
        static_assert((size) >= 0, "Záporná velikost bit. pole");              \
        for (bitset_index_t i = 0; i < _bitset_size(size); i++) {              \
            name[i] = 0;                                                       \
        }                                                                      \
        name[0] = (size);                                                      \
    }

// retyping size so that the compiler doesn't complain about an assertion
// being always true when the type of `size` is unsigned
#define bitset_alloc(name, size)                                               \
    bitset_t name;                                                             \
    {                                                                          \
        assert((ll)(size) >= 0);                                               \
        name = calloc(_bitset_size(size), sizeof(ul));                         \
        if (name == NULL) {                                                    \
            fputs("bitset_alloc: Chyba alokace paměti", stderr);               \
            exit(1);                                                           \
        }                                                                      \
        name[0] = (size);                                                      \
    }

#ifdef USE_INLINE

extern inline void bitset_free(bitset_t bitset) { free(bitset); }

extern inline ul bitset_size(bitset_t bitset) { return bitset[0]; }

extern inline void bitset_setbit(bitset_t bitset, bitset_index_t idx, ul val) {
    if (idx >= bitset[0] || (ll)idx < 0) {
        error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu", idx,
                   bitset[0]);
    }

    if (val) {
        bitset[_arr_idx(idx)] |= 1UL << _bit_idx(idx);
    } else {
        bitset[_arr_idx(idx)] &= ~(1UL << _bit_idx(idx));
    }
}

extern inline char bitset_getbit(bitset_t bitset, bitset_index_t idx) {
    if ((idx) >= bitset[0] || (ll)idx < 0) {
        error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu", idx,
                   bitset[0]);
    }

    return (bitset[_arr_idx(idx)] >> _bit_idx(idx)) & 1;
}

#else // USE_INLINE

#define bitset_free(name) (free(name))

#define bitset_size(name) (name[0])

#define bitset_setbit(name, idx, val)                                          \
    {                                                                          \
        if ((ul)(idx) >= name[0] || (ll)(idx) < 0) {                           \
            error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu",          \
                       (ul)(idx), name[0]);                                    \
        }                                                                      \
        if (val) {                                                             \
            name[_arr_idx(idx)] |= 1UL << _bit_idx(idx);                       \
        } else {                                                               \
            name[_arr_idx(idx)] &= ~(1UL << _bit_idx(idx));                    \
        }                                                                      \
    }

#define bitset_getbit(name, idx)                                               \
    (((ul)(idx) >= name[0] || (ll)(idx) < 0)                                   \
         ? error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu",           \
                      (ul)(idx), name[0])                                      \
         : (void)0,                                                            \
     (char)((name[_arr_idx(idx)] >> _bit_idx(idx)) & 1))

#endif // USE_INLINE

#endif // BITSET_MODULE
