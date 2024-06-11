// Filename: htab_find.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#include "htab_internal.h"

htab_pair_t *htab_find(const htab_t *htab, htab_key_t key) {
    size_t idx = htab_hash_function(key) % htab->arr_size;
    htab_item_t *item = htab->arr_ptr[idx];

    while (item != NULL) {
        if (strcmp(item->pair.key, key) == 0)
            return &item->pair;
        item = item->next;
    }

    return NULL;
}
