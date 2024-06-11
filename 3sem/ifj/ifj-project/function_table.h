/*
 * function_table.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef FUNCTION_TABLE_H
#define FUNCTION_TABLE_H

#include "global_include.h"
#include "sem_common.h"

typedef struct {
        Token *name;       // NULL if parameter is unnamed
        Token *ident;      // NULL if parameter is unused (no identifier)
        unsigned ident_id; // globally unique, used for codegen of calls
        Type type;
} FunctionParam;

typedef struct {
        Token *name;
        struct {
                FunctionParam *array;
                unsigned count;
                unsigned capacity;
        } params;
        Type return_type;
        bool is_void;
        bool is_builtin;
} FunctionSig;

typedef struct {
        FunctionSig *functions;
        unsigned count;
        unsigned capacity;
} FunctionTable;

/// initializes function table
void fntab_init();

/// adds new function to the table
/// returns ptr to newly created function
FunctionSig *fntab_add_func(Token *name);

/// adds parameter to last added function
void fntab_add_param(Token *name, Token *ident, Type type, unsigned ident_id);

/// adds return type to last added function
void fntab_add_return_type(Type return_type);

/// returns a pointer to function signature with given name, or NULL if such
/// name is not found
FunctionSig *fntab_lookup(char *name);

/// returns a pointer to function parameter info on given position (indexed
/// from zero) of given function, or NULL if parameter isn't found
FunctionParam *fntab_lookup_param(FunctionSig *func, unsigned position);

/// returns a pointer to function, in which we currently are
/// (does not check whether we actually are in a function, caller must check
/// that)
FunctionSig *fntab_current();

/// deletes function table and frees all allocated memory
void fntab_destroy();

#endif // FUNCTION_TABLE_H
