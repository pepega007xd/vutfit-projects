/*
 * print_tokens.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "global_include.h"
#include "token_table.h"
#include "tokenizer.h"
#include <stdio.h>

char *token_name(Token *token) {
    switch (token->token_type) {
    case tt_newl:
        return "tt_newl";
    case tt_EOF:
        return "tt_EOF";
    case tt_kw_Double:
        return "tt_kw_Double";
    case tt_kw_Double_opt:
        return "tt_kw_Double_opt";
    case tt_kw_else:
        return "tt_kw_else";
    case tt_kw_func:
        return "tt_kw_func";
    case tt_kw_if:
        return "tt_kw_if";
    case tt_kw_Int:
        return "tt_kw_Int";
    case tt_kw_Int_opt:
        return "tt_kw_Int_opt";
    case tt_kw_let:
        return "tt_kw_let";
    case tt_kw_nil:
        return "tt_kw_nil";
    case tt_kw_return:
        return "tt_kw_return";
    case tt_kw_String:
        return "tt_kw_String";
    case tt_kw_String_opt:
        return "tt_kw_String_opt";
    case tt_kw_var:
        return "tt_kw_var";
    case tt_kw_while:
        return "tt_kw_while";
    case tt_kw_Bool:
        return "tt_kw_Bool";
    case tt_kw_Bool_opt:
        return "tt_kw_Bool_opt";
    case tt_kw_true:
        return "tt_kw_true";
    case tt_kw_false:
        return "tt_kw_false";
    case tt_ident:
        return "tt_ident";
    case tt_lit_Int:
        return "tt_lit_Int";
    case tt_lit_Double:
        return "tt_lit_Double";
    case tt_lit_String:
        return "tt_lit_String";
    case tt_lit_Bool:
        return "tt_lit_Bool";
    case tt_left_paren:
        return "tt_left_paren";
    case tt_right_paren:
        return "tt_right_paren";
    case tt_colon:
        return "tt_colon";
    case tt_arrow:
        return "tt_arrow";
    case tt_left_curly:
        return "tt_left_curly";
    case tt_right_curly:
        return "tt_right_curly";
    case tt_assign:
        return "tt_assign";
    case tt_underscore:
        return "tt_underscore";
    case tt_comma:
        return "tt_comma";
    case tt_plus:
        return "tt_plus";
    case tt_minus:
        return "tt_minus";
    case tt_times:
        return "tt_times";
    case tt_divide:
        return "tt_divide";
    case tt_option:
        return "tt_option";
    case tt_yolo:
        return "tt_yolo";
    case tt_equals:
        return "tt_equals";
    case tt_not_equals:
        return "tt_not_equals";
    case tt_less_than:
        return "tt_less_than";
    case tt_greater_than:
        return "tt_greater_than";
    case tt_less_equals:
        return "tt_less_equals";
    case tt_greater_equals:
        return "tt_greater_rquals";
    }
    return "did i forget some tokens?";
}

void token_content(Token *token) {
    switch (token->token_type) {
    case tt_ident:
        fprintf(stderr, "'%s'\n", token->content.ident);
        return;

    case tt_lit_Int:
        fprintf(stderr, "'%d'\n", token->content.int_lit);
        return;

    case tt_lit_Double:
        fprintf(stderr, "'%lf'\n", token->content.double_lit);
        return;
    case tt_lit_String:
        fprintf(stderr, "'%s'\n", token->content.string_lit);
        return;
    case tt_lit_Bool:
        fprintf(stderr, "'%s'\n", token->content.bool_lit ? "true" : "false");
        return;
    default:
        fprintf(stderr, "\n");
    }
}

/// prints all tokens straight from tokenizer
void print_tokens() {
    Token token;
    while (true) {
        token = get_token();
        fprintf(stderr, "%s\t", token_name(&token));
        token_content(&token);
        if (token.token_type == tt_EOF) {
            break;
        }
    }
}

/// prints all tokens in token table, useful for debugging
void print_token_table() {
    token_table_start();
    fprintf(stderr, "---- TEST PRINT OF TOKEN TABLE:\n");

    Token *token = token_peek();

    while (token->token_type != tt_EOF) {
        fprintf(stderr, "%s\t", token_name(token));
        token_content(token);
        token_next();
        token = token_peek();
    }

    fprintf(stderr, "%s\n", token_name(token)); // also print EOF token
    fprintf(stderr, "---- END OF TOKEN TABLE\n");
}
