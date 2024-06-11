// Filename: htab_free.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#include "htab.h"
#include "htab_internal.h"
#include <stdlib.h>

void htab_free(htab_t *htab) {
    htab_clear(htab);
    free(htab->arr_ptr);
    free(htab);
}
