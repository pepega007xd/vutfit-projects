/*
 * print_tokens.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef PRINT_TOKENS_H
#define PRINT_TOKENS_H

#include "token_table.h"

/// returns the name of the token's type as string
char *token_name(Token *token);

/// prints all tokens straight from tokenizer
void print_tokens();

/// prints all tokens in token table, useful for debugging
void print_token_table();

#endif // PRINT_TOKENS_H
