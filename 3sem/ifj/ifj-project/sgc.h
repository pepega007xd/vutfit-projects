/*
 * sgc.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef SGC_H
#define SGC_H

#include <stdlib.h>

/// Initializes the global allocation data structures.
void sgc_init();

/// Works just like malloc.
void *sgc_malloc(size_t size);

/// Works just like calloc.
void *sgc_calloc(size_t nmemb, size_t size);

/// Works just like realloc.
void *sgc_realloc(void *ptr, size_t size);

/// works just like free.
void sgc_free(void *ptr);

/// Deallocates everything allocated to this point.
/// Returns the number of freed allocations.
unsigned sgc_free_all();

/// Deallocates global allocation data structures.
void sgc_destroy();

// these prevent anyone from using the original stdlib.h functions,
// since their allocations wouldn't be written into the alloc_table
#define malloc(x)                                                              \
    (void *)sizeof(struct {                                                    \
        _Static_assert(0,                                                      \
                       "You used the wrong malloc, use sgc_malloc instead");   \
        int c_does_not_permit_empty_structs_for_some_reason;                   \
    })

#define realloc(x, y)                                                          \
    (void *)sizeof(struct {                                                    \
        _Static_assert(0,                                                      \
                       "You used the wrong realloc, use sgc_realloc instead"); \
        int c_does_not_permit_empty_structs_for_some_reason;                   \
    })

#define calloc(x, y)                                                           \
    (void *)sizeof(struct {                                                    \
        _Static_assert(0,                                                      \
                       "You used the wrong calloc, use sgc_calloc instead");   \
        int c_does_not_permit_empty_structs_for_some_reason;                   \
    })

#define free(x)                                                                \
    (void)sizeof(struct {                                                      \
        _Static_assert(0, "You used the wrong free, use sgc_free instead");    \
        int c_does_not_permit_empty_structs_for_some_reason;                   \
    })

#endif // !SGC_H
