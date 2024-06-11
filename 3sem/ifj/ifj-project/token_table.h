/*
 * token_table.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef TOKEN_TABLE_H
#define TOKEN_TABLE_H

#include "global_include.h"
#include "tokenizer.h"

typedef struct {
        Token *tokens;
        unsigned size;
        unsigned capacity;
        int ptr_idx;
} TokenTable;

/// Initiazes the token table, and fills it using tokenizer.
void init_token_table();

/// Moves internal pointer to the first token.
void token_table_start();

/// Moves internal pointer to the next token.
void token_next();

/// Moves internal pointer to the previous token.
void token_prev();

/// Returns pointer to current token.
/// Returns NULL if you are on the end of table.
Token *token_peek();

#endif // TOKEN_TABLE_H
