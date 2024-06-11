// Filename: htab_erase.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#include "htab.h"
#include "htab_internal.h"
#include <stdlib.h>

bool htab_erase(htab_t *htab, htab_key_t key) {
    size_t idx = htab_hash_function(key) % htab->arr_size;
    htab_item_t *item = htab->arr_ptr[idx];
    htab_item_t **prev_ref = &htab->arr_ptr[idx];

    while (item != NULL) {
        if (strcmp(item->pair.key, key) == 0) {
            htab_item_t *next_ref = item->next;

            free((char *)item->pair.key);
            free(item);

            *prev_ref = next_ref;
            htab->size--;
            return true;
        }

        prev_ref = &item;
        item = item->next;
    }

    return false;
}
