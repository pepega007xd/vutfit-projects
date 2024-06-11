// Filename: htab_clear.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#include "htab.h"
#include "htab_internal.h"
#include <stdlib.h>

void htab_clear(htab_t *htab) {
    for (size_t i = 0; i < htab->arr_size; i++) {
        htab_item_t *item = htab->arr_ptr[i];

        while (item != NULL) {
            htab_item_t *next_item = item->next;
            free((char *)item->pair.key);
            free(item);
            item = next_item;
        }

        htab->arr_ptr[i] = NULL;
    }

    htab->size = 0;
}
