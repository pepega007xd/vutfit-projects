/*
 * test-symtable.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "symtable.h"

// `Type` in `VarInfo` is an enum (represented by an int) => the whole struct
// must be bigger than an int => this is legit
VarInfo mock(unsigned data) {
    VarInfo var_info;
    unsigned *unsignedptr = (unsigned *)&var_info;
    *unsignedptr = data;
    return var_info;
}

// same thing
unsigned unmock(VarInfo *var_info) { return *(unsigned *)var_info; }

void test_htab_init() {
    HashTable table = hash_table_init(4);
    hash_table_dispose(&table);
}

void test_htab_insert_find() {
    HashTable table = hash_table_init(4);
    hash_table_insert(&table, "kentus", mock(42));
    hash_table_insert(&table, "kentus blentus", mock(666));
    hash_table_insert(&table, "abc", mock(10));
    hash_table_insert(&table, "", mock(0));
    hash_table_insert(&table, "AAAAAAAAAAAAAAAAAAAAAAAAAHHHHHHHHHHHH",
                      mock(2023));

    assert(unmock(hash_table_find(table, "kentus")) == 42);
    assert(unmock(hash_table_find(table, "kentus blentus")) == 666);
    assert(unmock(hash_table_find(table, "abc")) == 10);
    assert(unmock(hash_table_find(table, "")) == 0);
    assert(unmock(hash_table_find(
               table, "AAAAAAAAAAAAAAAAAAAAAAAAAHHHHHHHHHHHH")) == 2023);

    // overwriting keys

    hash_table_insert(&table, "kentus", mock(89));
    hash_table_insert(&table, "kentus blentus", mock(16));
    hash_table_insert(&table, "abc", mock(12345));
    hash_table_insert(&table, "", mock(1));
    hash_table_insert(&table, "AAAAAAAAAAAAAAAAAAAAAAAAAHHHHHHHHHHHH",
                      mock(2023));

    assert(unmock(hash_table_find(table, "kentus")) == 89);
    assert(unmock(hash_table_find(table, "kentus blentus")) == 16);
    assert(unmock(hash_table_find(table, "abc")) == 12345);
    assert(unmock(hash_table_find(table, "")) == 1);
    hash_table_insert(&table, "AAAAAAAAAAAAAAAAAAAAAAAAAHHHHHHHHHHHH",
                      mock(2024));

    // nonexistent keys

    assert(hash_table_find(table, "blentus kentus") == NULL);
    assert(hash_table_find(table, "acb") == NULL);
    assert(hash_table_find(table, " ") == NULL);
    assert(hash_table_find(table, "ketnus") == NULL);
    assert(hash_table_find(table, ":copium:") == NULL);

    hash_table_dispose(&table);
}

void test_htab_insert_many() {
    unsigned testsize = 1000;
    // this should make minimal size (4)
    HashTable table = hash_table_init(0);
    char **keys = sgc_malloc(sizeof(void *) * testsize);

    for (unsigned i = 0; i < testsize; i++) {
        keys[i] = sgc_malloc(i + 1);

        for (unsigned j = 0; j < i; j++) {
            keys[i][j] = (char)(rand() % 254) + 1;
        }
        keys[i][i] = 0;

        hash_table_insert(&table, keys[i], mock(i));
    }

    assert(unmock(hash_table_find(table, keys[0])) == 0);

    for (unsigned i = 1; i < testsize; i++) {
        assert(unmock(hash_table_find(table, keys[i])) == i);
    }

    hash_table_dispose(&table);

    for (unsigned i = 0; i < testsize; i++) {
        sgc_free(keys[i]);
    }

    sgc_free(keys);
}

int main(void) {
    sgc_init();

    test_htab_init();
    test_htab_insert_find();
    test_htab_insert_many();

    assert(sgc_free_all() == 0);
    return 0;
}
