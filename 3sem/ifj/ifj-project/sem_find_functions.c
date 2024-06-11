/*
 * sem_find_functions.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "sem_find_functions.h"
#include "function_table.h"
#include "global_include.h"
#include "rec_parser.h"
#include "token_table.h"

void find_functions() {
    Token *token = token_peek();
    while (token->token_type != tt_EOF) {
        // skip all the way to tt_kw_func
        if (token->token_type != tt_kw_func) {
            token_next();
            token = token_peek();
            continue;
        }

        // this will parse the function header up to
        // the beginning of the code block, and put information
        // about the function into function table
        parse_stmt_func_def(true);
        token = token_peek();
    }

    // rewinds token table back to start
    token_table_start();
}

void sem_func_def_name(Token *func_name) {
    // check if function already exists
    FunctionSig *existing = fntab_lookup(func_name->content.ident);
    if (existing != NULL) {
        error_print_exit(func_name, error_other_semantic,
                         "Redefinition of function.");
    }

    fntab_add_func(func_name);
}

void sem_func_def_param(Token *name, Token *ident, Token *type) {
    unsigned ident_id = sem_id_get();
    Type param_type = sem_type_get(type);

    // in function table, we represent empty name/ident as NULL
    if (name->token_type == tt_underscore)
        name = NULL;
    if (ident->token_type == tt_underscore)
        ident = NULL;

    // name and identifier must not be the same
    if (name != NULL && ident != NULL) {
        if (!strcmp(name->content.ident, ident->content.ident))
            error_print_exit(
                name, error_other_semantic,
                "Parameter name must differ from parameter identifier.");
    }

    fntab_add_param(name, ident, param_type, ident_id);
}

void sem_func_def_return_type(Token *type) {
    if (type != NULL) {
        fntab_add_return_type(sem_type_get(type));
    }
}
