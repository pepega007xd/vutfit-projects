// Filename: htab_hash_function.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#include "htab_internal.h"
#include <inttypes.h>

// "sdbm" variant from http://www.cse.yorku.ca/~oz/hash.html
size_t htab_hash_function(const char *str) {
    uint32_t h = 0; // musí mít 32 bitů
    const unsigned char *p;
    for (p = (const unsigned char *)str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}
