/*
 * sem_state.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef SEM_STATE_H
#define SEM_STATE_H

//! contains global state of semantic analyzer, and an API for changing it,
//! this consists of the whole symbol table, and information about whether
//! we're currently processing a function body, or global code

#include "global_include.h"
#include "symtable.h"

typedef struct {
        HashTable *scopes;
        unsigned size;
        unsigned capacity;
} ScopeStack;

typedef struct {
        ScopeStack globals;
        ScopeStack locals;
        bool in_local;
} SemState;

/// initializes the global state of semantic analyzer
void sem_state_init();

/// checks whether variable can be added to current scope, and adds it
/// `name` is the token of variable's name in declaration
/// `type` is the variable type
/// `is_mutable` says, if it is declared as mutable (var), or immutable (let)
/// `is_initialized` says, if the variable is initialized right after creation
VarInfo *sem_var_add(Token *name, Type type, bool is_mutable,
                     bool is_initialized);

/// finds variable according to this algorithm:
/// If we're in a local scope (inside function body), first look through all
/// local scopes from the lowest one, then (in every case) look into the highest
/// global scope. Returns NULL when variable isn't found.
VarInfo *sem_var_lookup(Token *var);

/// creates new scope inside current one (local or global)
void sem_scope_enter();

/// removes current scope
void sem_scope_leave();

/// goes from global scope to local scope
void sem_function_enter();

/// goes from local scope to global scope
void sem_function_leave();

/// returns true we are in a local frame (inside a function body)
bool sem_in_local();

/// returns, in how many scopes are we nested (zero for top scope)
unsigned sem_scope_depth();

/// deallocates data structures of semantic analyzer
void sem_state_destroy();

#endif // SEM_STATE_H
