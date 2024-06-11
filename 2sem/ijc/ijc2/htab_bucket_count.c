// Filename: htab_bucket_count.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#include "htab_internal.h"

size_t htab_bucket_count(const htab_t *htab) { return htab->arr_size; }
