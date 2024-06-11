/*
 * toke_table.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "token_table.h"
#include "global_include.h"
#include "tokenizer.h"
#include <assert.h>

TokenTable token_table;

void add_token(Token token, bool after_newline) {
    token.after_newline = after_newline;
    if (token_table.size == token_table.capacity) {
        // add one sizeof(Token) extra for increasing capacity when adding
        // first token
        token_table.capacity = token_table.capacity * 2 + 1;
        token_table.tokens = sgc_realloc(token_table.tokens,
                                         token_table.capacity * sizeof(Token));
    }

    token_table.tokens[token_table.size++] = token;
}

void init_token_table() {
    token_table.tokens = NULL;
    token_table.size = 0;
    token_table.capacity = 0;
    // -1 index means past end / before start / empty table
    token_table.ptr_idx = -1;

    tokenizer_init();

    Token token;
    bool after_newline = false;
    while (true) {
        token = get_token();
        if (token.token_type == tt_newl) {
            after_newline = true;
        } else if (token.token_type == tt_EOF) {
            add_token(token, after_newline);
            break;
        } else {
            add_token(token, after_newline);
            after_newline = false;
        }
    }

    // initialize table to first token if there is any
    if (token_table.size != 0) {
        token_table.ptr_idx = 0;
    }
}

void token_table_start() {
    if (token_table.size != 0)
        token_table.ptr_idx = 0;
}

void token_next() {
    // check if index is valid
    if (token_table.ptr_idx == -1) {
        return;
    }

    // check if we're not on the end of table
    if ((int)token_table.size != token_table.ptr_idx + 1) {
        token_table.ptr_idx++;
    }
}

void token_prev() {
    // check if index is valid
    if (token_table.ptr_idx == -1) {
        return;
    }

    // check if we're not on the start of table
    if (token_table.ptr_idx == 0) {
        token_table.ptr_idx = -1;
    } else {
        token_table.ptr_idx--;
    }
}

Token *token_peek() {
    // check if index is valid
    if (token_table.ptr_idx == -1) {
        return NULL;
    }

    return &token_table.tokens[token_table.ptr_idx];
}
