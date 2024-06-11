// Filename: htab_init.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#include "htab.h"
#include "htab_internal.h"
#include <stdlib.h>

htab_t *htab_init(const size_t n) {
    htab_t *htab = malloc(sizeof(htab_t));
    if (htab == NULL)
        goto ERR_EXIT;

    htab->size = 0;
    htab->arr_size = n;

    htab->arr_ptr = calloc(n, sizeof(htab_item_t *));
    if (htab->arr_ptr == NULL)
        goto ERR_FREE_HTAB;

    return htab;

ERR_FREE_HTAB:
    free(htab);

ERR_EXIT:
    return NULL;
}
