// Filename: htab_lookup_add.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#include "htab.h"
#include "htab_internal.h"
#include <stdlib.h>
#include <string.h>

htab_item_t *create_item(htab_key_t key) {
    htab_item_t *new_item = malloc(sizeof(htab_item_t));
    if (new_item == NULL)
        goto ERR_EXIT;

    new_item->next = NULL;
    new_item->pair.value = 0;

    new_item->pair.key = malloc(strlen(key) + 1);
    if (new_item->pair.key == NULL)
        goto ERR_FREE_ITEM;

    strcpy((char *)new_item->pair.key, key);

    return new_item;

ERR_FREE_ITEM:
    free(new_item);

ERR_EXIT:
    return NULL;
}

htab_pair_t *htab_lookup_add(htab_t *htab, htab_key_t key) {
    size_t idx = htab_hash_function(key) % htab->arr_size;
    htab_item_t *item = htab->arr_ptr[idx];

    if (item == NULL) {
        htab->arr_ptr[idx] = create_item(key);
        if (htab->arr_ptr[idx] == NULL)
            return NULL;
        htab->size++;
        return &htab->arr_ptr[idx]->pair;
    }

    while (1) {
        if (strcmp(item->pair.key, key) == 0)
            return &item->pair;

        if (item->next == NULL) {
            item->next = create_item(key);
            if (item->next == NULL)
                return NULL;
            htab->size++;
            return &item->next->pair;
        }

        item = item->next;
    }
}
