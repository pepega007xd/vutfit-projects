// Filename: htab_internal.h
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#ifndef HTAB_INTERNAL_H
#define HTAB_INTERNAL_H

#include "htab.h"

typedef struct htab_item {
    struct htab_item *next;
    htab_pair_t pair;
} htab_item_t;

struct htab {
    size_t size;
    size_t arr_size;
    htab_item_t **arr_ptr;
};

#endif // HTAB_INTERNAL_H
