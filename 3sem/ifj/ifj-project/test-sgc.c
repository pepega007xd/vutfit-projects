/*
 * test-sgc.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "global_include.h"
#include "sgc.h"
#include <assert.h>

void test_malloc() {
    int *intarray = sgc_malloc(sizeof(int[20]));
    assert(intarray != NULL);

    intarray[5] = 42;
    assert(intarray[5] == 42);
    sgc_free(intarray);
}

void test_calloc() {
    int *intarray = sgc_calloc(1, sizeof(int[20]));
    assert(intarray != NULL);

    for (unsigned i = 0; i < 20; i++) {
        assert(intarray[i] == 0);
    }

    intarray[5] = 42;
    assert(intarray[5] == 42);

    sgc_free(intarray);
}

void test_realloc() {
    int *intarray = sgc_malloc(sizeof(int[2]));
    intarray = sgc_realloc(intarray, 200);
    intarray[19] = 42;
    assert(intarray[19] == 42);
    sgc_free(intarray);

    // test is realloc with NULL ptr behaves like malloc
    char *arr = sgc_realloc(NULL, 12);
    arr[11] = 42;
    assert(arr[11] == 42);
}

void test_free_all() {
    for (unsigned i = 0; i < 1000; i++) {
        sgc_malloc(sizeof(int[10]));
    }

    sgc_free_all();
}

int main() {
    sgc_init();
    test_malloc();
    test_calloc();
    test_realloc();
    test_free_all();
    sgc_destroy();

    return 0;
}
