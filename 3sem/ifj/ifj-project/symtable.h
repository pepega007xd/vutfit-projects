/*
 * symtable.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "global_include.h"
#include "sem_common.h"

typedef struct {
        bool is_mutable;
        bool is_initialized;
        bool is_local;
        Type type;
        Token *declaration_token;
        unsigned id;
        unsigned scope_depth;
} VarInfo;

typedef struct {
        char *key;
        VarInfo value;
} HashTableItem;

typedef struct {
        unsigned size;
        unsigned capacity;
        HashTableItem *items;
} HashTable;

/// initializes hashtable, mininum initial size is 4
HashTable hash_table_init(unsigned hash_table_size);

/// returns a pointer to value associated with key, or NULL if key is not found
VarInfo *hash_table_find(HashTable table, char *key);

/// inserts new value with provided key into table, or overwrites existing
/// value, if such key it already exists
///
/// key string is not copied, it is assumed to remain unchanged for the
/// existence of the table
///
/// returns pointer to inserted value
VarInfo *hash_table_insert(HashTable *table, char *key, VarInfo value);

/// deallocates all memory associated with hashtable
void hash_table_dispose(HashTable *table);

#endif // SYMTABLE_H
