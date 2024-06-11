/*
 * function_table.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "function_table.h"
#include "global_include.h"
#include "sgc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

FunctionTable function_table;

#define FAKE_TOKEN_COUNT 50

Token fake_tokens[FAKE_TOKEN_COUNT];

Token *get_fake_token() {
    static unsigned used_tokens;
    if (used_tokens == FAKE_TOKEN_COUNT - 1) {
        assert(false); // increase FAKE_TOKEN_COUNT, add_builtin_functions needs
                       // more tokens
    }
    return &fake_tokens[used_tokens++];
}

// allocates fake token for function name, registers function into table
#define MK_FUNC(name)                                                          \
    {                                                                          \
        Token *name = get_fake_token();                                        \
        name->token_type = tt_ident;                                           \
        name->content.ident = #name;                                           \
        FunctionSig *func = fntab_add_func(name);                              \
        /* this disables appending IDs to function params */                   \
        func->is_builtin = true;                                               \
    }

// allocates fake token for function ident
// separate scope is because the parameter idents would be redefined
#define MK_PARAM(param_ident, type)                                            \
    {                                                                          \
        Token *param_ident = get_fake_token();                                 \
        param_ident->token_type = tt_ident;                                    \
        /* all builtin functions have param idents prefixed with $ */          \
        param_ident->content.ident = "$" #param_ident;                         \
        fntab_add_param(NULL, param_ident, type, sem_id_get());                \
    }

#define MK_NAMED_PARAM(name, param_ident, type)                                \
    {                                                                          \
        Token *name = get_fake_token();                                        \
        name->token_type = tt_ident;                                           \
        name->content.ident = #name;                                           \
                                                                               \
        Token *param_ident = get_fake_token();                                 \
        param_ident->token_type = tt_ident;                                    \
        /* all builtin functions have param idents prefixed with $ */          \
        param_ident->content.ident = "$" #param_ident;                         \
        fntab_add_param(name, param_ident, type, sem_id_get());                \
    }

void add_builtin_functions() {
    MK_FUNC(readString);
    fntab_add_return_type(type_string_opt);

    MK_FUNC(readInt);
    fntab_add_return_type(type_int_opt);

    MK_FUNC(readDouble);
    fntab_add_return_type(type_double_opt);

    MK_FUNC(Int2Double);
    MK_PARAM(term, type_int);
    fntab_add_return_type(type_double);

    MK_FUNC(Double2Int);
    MK_PARAM(term, type_double);
    fntab_add_return_type(type_int);

    MK_FUNC(length);
    MK_PARAM(s, type_string);
    fntab_add_return_type(type_int);

    MK_FUNC(substring);
    MK_NAMED_PARAM(of, s, type_string);
    MK_NAMED_PARAM(startingAt, i, type_int);
    MK_NAMED_PARAM(endingBefore, j, type_int);
    fntab_add_return_type(type_string_opt);

    MK_FUNC(ord);
    MK_PARAM(c, type_string);
    fntab_add_return_type(type_int);

    MK_FUNC(chr);
    MK_PARAM(i, type_int);
    fntab_add_return_type(type_string);
}

void fntab_init() {
    function_table.functions = NULL;
    function_table.count = 0;
    function_table.capacity = 0;

    add_builtin_functions();
}

FunctionSig *fntab_add_func(Token *name) {
    // reallocate if needed
    if (function_table.count == function_table.capacity) {
        function_table.capacity = function_table.capacity * 2 + 1;
        function_table.functions =
            sgc_realloc(function_table.functions,
                        function_table.capacity * sizeof(FunctionSig));
    }

    FunctionSig *new_func = &function_table.functions[function_table.count];
    new_func->name = name;
    new_func->is_void = true;
    new_func->params.array = NULL;
    new_func->params.count = 0;
    new_func->params.capacity = 0;
    new_func->is_builtin = false;

    function_table.count++;
    return new_func;
}

void fntab_add_param(Token *name, Token *ident, Type type, unsigned ident_id) {
    FunctionSig *func = &function_table.functions[function_table.count - 1];

    // first look for parameter ident collision
    for (unsigned i = 0; i < func->params.count; i++) {
        FunctionParam prev_param = func->params.array[i];
        if (!strcmp(prev_param.ident->content.ident, ident->content.ident)) {
            error_print_exit(ident, error_other_semantic,
                             "Redefinition of parameter (same identifier).");
        }
    }

    // reallocate if needed
    if (func->params.count == func->params.capacity) {
        func->params.capacity = func->params.capacity * 2 + 1;
        func->params.array = sgc_realloc(
            func->params.array, func->params.capacity * sizeof(FunctionParam));
    }

    func->params.array[func->params.count].name = name;
    func->params.array[func->params.count].type = type;
    func->params.array[func->params.count].ident = ident;
    func->params.array[func->params.count].ident_id = ident_id;
    func->params.count++;
}

FunctionSig *fntab_lookup(char *name) {
    for (unsigned i = 0; i < function_table.count; i++) {
        // comparing input name and name token from declaration
        if (!strcmp(name, function_table.functions[i].name->content.ident)) {
            return &function_table.functions[i];
        }
    }

    return NULL;
}

FunctionParam *fntab_lookup_param(FunctionSig *func, unsigned position) {
    if (position < func->params.count) {
        return &func->params.array[position];
    } else {
        return NULL;
    }
}

void fntab_add_return_type(Type return_type) {
    function_table.functions[function_table.count - 1].return_type =
        return_type;
    function_table.functions[function_table.count - 1].is_void = false;
}

FunctionSig *fntab_current() {
    return &function_table.functions[function_table.count - 1];
}

void fntab_destroy() {
    for (unsigned i = 0; i < function_table.count; i++) {
        sgc_free(function_table.functions[i].params.array);
    }
    sgc_free(function_table.functions);
}
