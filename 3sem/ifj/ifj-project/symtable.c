/*
 * symtable.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "symtable.h"
#include <assert.h>

// source: i made it up
unsigned hash_function(char *str) {
    unsigned hash = 47869; // prime

    for (int i = 0; str[i] != 0; i++) {
        unsigned c = (unsigned)str[i];
        hash = (((hash - c) << 3) + hash) + c;
    }

    return hash;
}

HashTable hash_table_init(unsigned hash_table_size) {
    // increase size if needed
    if (hash_table_size < 4) {
        hash_table_size = 4;
    }

    HashTable table;
    table.size = 0;
    // calloc => every unused key pointer is NULL
    table.items = sgc_calloc(hash_table_size, sizeof(HashTableItem));
    table.capacity = hash_table_size;

    return table;
}

VarInfo *hash_table_find(HashTable table, char *key) {
    unsigned idx = hash_function(key) % table.capacity;

    while (table.items[idx].key != NULL) {
        if (strcmp(key, table.items[idx].key))
            idx = (idx + 1) % table.capacity;
        else
            return &table.items[idx].value;
    }

    // return value was not found
    return NULL;
}

// inserts element into table, doesn't check for overfilling
VarInfo *hash_table_insert_unchecked(HashTable *table, char *key,
                                     VarInfo value) {
    unsigned idx = hash_function(key) % table->capacity;

    while (table->items[idx].key != NULL) {
        if (!strcmp(table->items[idx].key, key)) {
            // key was found, replace value
            table->items[idx].value = value;
            return &table->items[idx].value;
        } else {
            idx = (idx + 1) % table->capacity;
        }
    }

    // key is not in table, create new item
    table->items[idx].key = key;
    table->items[idx].value = value;
    table->size++;
    return &table->items[idx].value;
}

// doubles the size of table
void hash_table_expand(HashTable *table) {
    unsigned new_cap = table->capacity * 2;
    HashTableItem *new_items = sgc_calloc(new_cap, sizeof(HashTableItem));

    // temporary table for use with hash_table_insert_unchecked
    HashTable tmptable = {.items = new_items, .capacity = new_cap, .size = 0};

    // copy items to new array
    for (unsigned i = 0; i < table->capacity; i++) {
        if (table->items[i].key != NULL) {
            hash_table_insert_unchecked(&tmptable, table->items[i].key,
                                        table->items[i].value);
        }
    }

    // swap the old array with new and free the old one
    HashTableItem *old_items = table->items;
    table->items = new_items;
    table->capacity = new_cap;
    sgc_free(old_items);
}

VarInfo *hash_table_insert(HashTable *table, char *key, VarInfo value) {
    // check if there is enough space in table,
    // and resize if needed
    if (table->size + 1 >= table->capacity / 2) {
        hash_table_expand(table);
    }

    return hash_table_insert_unchecked(table, key, value);
}

void hash_table_dispose(HashTable *table) {
    sgc_free(table->items);
    table->items = NULL;
    table->size = 0;
    table->capacity = 0;
}
