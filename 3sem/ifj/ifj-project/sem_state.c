/*
 * sem_state.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "sem_state.h"
#include "global_include.h"
#include "sem_common.h"
#include "sgc.h"
#include "symtable.h"
#include <assert.h>
#include <stdio.h>

unsigned htab_default_size = 31; // prime

SemState sem_state;

void sem_state_init() {
    sem_state.globals.size = 1;
    sem_state.globals.capacity = 1;
    sem_state.globals.scopes = sgc_malloc(sizeof(HashTable));
    sem_state.globals.scopes[0] = hash_table_init(htab_default_size);

    sem_state.locals.size = 0;
    sem_state.locals.capacity = 0;
    sem_state.locals.scopes = NULL;

    sem_state.in_local = false;
}

VarInfo *sem_var_add(Token *name, Type type, bool is_mutable,
                     bool is_initialized) {
    ScopeStack stack =
        sem_state.in_local ? sem_state.locals : sem_state.globals;

    VarInfo *existing = sem_var_lookup(name);

    // if there already is a variable in this scope with this name
    if (existing != NULL && existing->scope_depth == stack.size - 1) {
        error_print_exit(name, error_ident, "Redefinition of variable.");
    }

    HashTable *scope = &stack.scopes[stack.size - 1];

    VarInfo var_info = {.scope_depth = stack.size - 1,
                        .id = sem_id_get(),
                        .type = type,
                        .is_mutable = is_mutable,
                        .is_initialized = is_initialized,
                        .is_local = sem_state.in_local,
                        .declaration_token = name};

    return hash_table_insert(scope, name->content.ident, var_info);
}

VarInfo *sem_var_lookup(Token *name) {
    char *varname = name->content.ident;
    // only search in local scopes if we are in a local scope
    if (sem_state.in_local) {
        for (int i = (int)sem_state.locals.size - 1; i >= 0; i--) {
            VarInfo *info =
                hash_table_find(sem_state.locals.scopes[i], varname);
            if (info != NULL)
                return info;
        }
    }

    // search in global scopes every time
    for (int i = (int)sem_state.globals.size - 1; i >= 0; i--) {
        VarInfo *info = hash_table_find(sem_state.globals.scopes[i], varname);
        if (info != NULL)
            return info;
    }

    return NULL;
}

void sem_scope_enter() {
    ScopeStack *stack =
        sem_state.in_local ? &sem_state.locals : &sem_state.globals;

    // reallocate if needed
    if (stack->size == stack->capacity) {
        stack->capacity = stack->capacity * 2 + 1;
        stack->scopes =
            sgc_realloc(stack->scopes, stack->capacity * sizeof(ScopeStack));
    }

    stack->scopes[stack->size++] = hash_table_init(htab_default_size);
}

void sem_scope_leave() {
    ScopeStack *stack =
        sem_state.in_local ? &sem_state.locals : &sem_state.globals;

    hash_table_dispose(&stack->scopes[--stack->size]);
}

unsigned sem_scope_depth() {
    ScopeStack *stack =
        sem_state.in_local ? &sem_state.locals : &sem_state.globals;
    return stack->size - 1;
}

void sem_function_enter() {
    assert(!sem_state.in_local); // func def inside another func def?
    sem_state.in_local = true;
    sem_scope_enter();
}

void sem_function_leave() {
    sem_scope_leave();
    assert(sem_state.in_local); // return outside of function body?
    sem_state.in_local = false;
}

bool sem_in_local() { return sem_state.in_local; }

void sem_state_destroy() {
    for (unsigned i = 0; i < sem_state.locals.size; i++) {
        hash_table_dispose(&sem_state.locals.scopes[i]);
    }

    for (unsigned i = 0; i < sem_state.globals.size; i++) {
        hash_table_dispose(&sem_state.globals.scopes[i]);
    }
}
