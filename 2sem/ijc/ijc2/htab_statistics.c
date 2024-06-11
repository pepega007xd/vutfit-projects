// Filename: htab_statistics.c
// Author: Tomas Brablec (xbrabl04)
// Faculty: FIT BUT
// Date: 2023-03-28

#include "htab.h"
#include "htab_internal.h"
#include <stdint.h>
#include <stdio.h>

#ifdef STATISTICS

void htab_statistics(const htab_t *htab) {
    size_t sum_lengths = 0;
    size_t min_length = SIZE_MAX;
    size_t max_length = 0;

    for (size_t i = 0; i < htab->arr_size; i++) {
        size_t length = 0;
        htab_item_t *item = htab->arr_ptr[i];

        while (item != NULL) {
            length++;
            item = item->next;
        }

        if (length < min_length)
            min_length = length;
        if (length > max_length)
            max_length = length;
        sum_lengths += length;
    }

    fprintf(stderr,
            "Minimum bucket size: %lu\n"
            "Maximum bucket size: %lu\n"
            "Average bucket size: %lf\n",
            min_length, max_length, sum_lengths / (double)htab->arr_size);
}

#endif // STATISTICS
