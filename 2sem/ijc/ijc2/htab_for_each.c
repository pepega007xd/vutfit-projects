// Filename: htab_for_each.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#include "htab.h"
#include "htab_internal.h"

void htab_for_each(const htab_t *htab, void (*fn)(htab_pair_t *)) {
    for (size_t i = 0; i < htab->arr_size; i++) {
        htab_item_t *item = htab->arr_ptr[i];
        while (item != NULL) {
            fn(&item->pair);
            item = item->next;
        }
    }
}
