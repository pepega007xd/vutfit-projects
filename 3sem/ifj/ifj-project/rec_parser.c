/*
 * rec_parser.c
 *
 * xkralo07 Anna Králová
 * xbrabl04 Tomáš Brablec
 *
 */

#include "rec_parser.h"
#include "global_include.h"
#include "print_tokens.h"
#include "sem_find_functions.h"
#include "sem_stmt.h"
#include <assert.h>
#include <stdio.h>

/// prints error about unexpected token and exits
#define unexpected_token(token)                                                \
    {                                                                          \
        fprintf(stderr, "Unexpected token '%s' on line %d in function %s\n",   \
                token_name(token), __LINE__, __func__);                        \
        error_print_exit(token, error_parser, "Unexpected token.");            \
    }

Token *expect_token(TokenType expected_token_type) {
    Token *token = token_peek();

    if (token->token_type != expected_token_type) {
        unexpected_token(token);
    }
    return token;
}

// prints nice error message in case we find wrong token type
#define expect_token(expected_type)                                            \
    ((token_peek()->token_type == expected_type)                               \
         ? token_peek()                                                        \
         : (fprintf(stderr,                                                    \
                    "Unexpected token '%s' on line %d in function %s\n",       \
                    token_name(token_peek()), __LINE__, __func__),             \
            error_print_exit(token_peek(), error_parser, "Unexpected token."), \
            NULL))

// this is impossible in LL1 parser, we must look and second token and
// check if its type is `tt_ident` or `tt_assign`, because when we're
// parsing the following pieces of code:
//
// return
// a = b
//
// or
//
// return
// a()
//
// the expression parser would falsely succeed and parse `a` as the
// returned expression, which would then trigger error when parsing the
// `= var2` or `()` residue
ExprInfo *parse_expr_opt() {
    // EXPR_OPT = EXPR
    // EXPR_OPT = EPSILON
    Token *first_token = token_peek();
    Token *second_token;
    switch (first_token->token_type) {
    case tt_ident:
        // look at second token, if it's tt_left_paren or tt_assign,
        // we found an exception, as mentioned above
        token_next();
        second_token = token_peek();
        token_prev(); // now we're back at first_token
        if (second_token->token_type == tt_left_paren ||
            second_token->token_type == tt_assign) {
            return NULL; // EPSILON
        }
        return parse_expr();
    case tt_lit_Int:
    case tt_lit_Double:
    case tt_lit_String:
    case tt_lit_Bool:
    case tt_kw_nil:
    case tt_left_paren:
        return parse_expr();
    default:
        return NULL; // not an expression
    }
}

// this is impossible to determine in LL1 parser, we must look at first
// two tokens, and if they're `tt_ident` and `tt_left_paren`
// respectively, it is a function call, otherwise, it is an expression,
// or an invalid input
ExprInfo *parse_expr_or_call() {
    // EXPR_OR_CALL = EXPR
    // EXPR_OR_CALL = ident CALL
    Token *first_token = token_peek();
    token_next();
    Token *second_token = token_peek();

    if (first_token->token_type == tt_ident &&
        second_token->token_type == tt_left_paren) {
        return parse_call(first_token); // passing the function name
    } else {
        token_prev(); // now we're at the start of expression
        return parse_expr();
    }
}

void parse_file() {
    // FILE = STMT_LIST
    parse_stmt_list();
    expect_token(tt_EOF);
}

bool parse_stmt_list_rec(
    bool is_first_stmt) // ε, ident, tt_kw_func, tt_kw_let, tt_kw_var,
                        // tt_kw_if, tt_kw_while, tt_kw_return
{
    // STMT_LIST = EPSILON
    // STMT_LIST = STMT STMT_LIST
    Token *token = token_peek();
    bool contains_ret;
    switch (token->token_type) // STMT
    {
    case tt_ident:
    case tt_kw_func:
    case tt_kw_let:
    case tt_kw_var:
    case tt_kw_if:
    case tt_kw_while:
    case tt_kw_return:
        if (!is_first_stmt && !token->after_newline) {
            error_print_exit(token, error_parser,
                             "Each statement must begin on a new line.");
        }

        contains_ret = parse_stmt();
        contains_ret |= parse_stmt_list_rec(false); // STMT_REST
        return contains_ret;

    default: // EPSILON
        return false;
    }
}

bool parse_stmt_list() { return parse_stmt_list_rec(true); }

bool parse_stmt() {
    // STMT = STMT_FUNC_DEF
    // STMT = STMT_LET
    // STMT = STMT_VAR
    // STMT = ident STMT_IDENT
    // STMT = STMT_IF
    // STMT = STMT_WHILE
    // STMT = STMT_RETURN
    // STMT = EPSILON
    Token *token;
    token = token_peek();
    switch (token->token_type) {
    case tt_kw_func:
        // during main pass, we don't want to call semantic analysis on the
        // function header, that was done in pre-pass
        parse_stmt_func_def(false);
        break;

    case tt_kw_let:
        parse_stmt_let();
        break;

    case tt_kw_var:
        parse_stmt_var();
        break;

    case tt_ident:
        token_next();
        parse_stmt_ident(token);
        break;

    case tt_kw_if:
        return parse_stmt_if();

    case tt_kw_while:
        parse_stmt_while();
        break;

    case tt_kw_return:
        parse_stmt_return();
        return true;

    default:
        return false; // EPSILON
    }
    return false;
}

void parse_stmt_func_def(bool call_sem_on_header) {
    // STMT_FUNC_DEF = tt_kw_func tt_ident
    // tt_left_paren PARAM_LIST tt_right_paren
    // RETURN_TYPE
    token_next();
    Token *func_name = expect_token(tt_ident);
    token_next();

    if (call_sem_on_header) {
        // pre-pass
        sem_func_def_name(func_name);
    } else {
        // main pass
        sem_func_def_begin(func_name);

        // this is for semantic analysis of return stmt, the analyzer needs to
        // know which function we're calling, and passing the function name
        // everywhere between parse_* calls would be too much boilerplate, so we
        // use extern global variable to set the name directly
        extern Token *called_func_name;
        called_func_name = func_name;
    }

    expect_token(tt_left_paren);
    token_next();
    parse_param_list(call_sem_on_header);
    expect_token(tt_right_paren);
    token_next();

    Token *return_type = parse_return_type();
    if (call_sem_on_header) {
        sem_func_def_return_type(return_type);
    }

    if (call_sem_on_header) {
        return; // we want to parse just the header during pre-pass
    }

    expect_token(tt_left_curly);
    token_next();
    bool contains_ret = parse_stmt_list(); // function body
    sem_func_def_end(func_name, contains_ret);
    expect_token(tt_right_curly);
    token_next();
}

void parse_stmt_let() {
    // STMT_LET = tt_kw_let ident DECL
    token_next();
    Token *ident = expect_token(tt_ident);
    token_next();
    Token *type = NULL;
    ExprInfo *expr_info = parse_decl(&type);
    sem_let(ident, type, expr_info);
}

void parse_stmt_var() {
    // STMT_VAR = tt_kw_var ident DECL
    token_next();
    Token *ident = expect_token(tt_ident);
    token_next();
    Token *type = NULL;
    ExprInfo *expr_info = parse_decl(&type);
    sem_var(ident, type, expr_info);
}

void parse_stmt_ident(Token *ident) {
    // STMT_IDENT = tt_assign EXPR_OR_CALL
    // STMT_IDENT = CALL

    Token *token = token_peek();
    ExprInfo *expr_info;

    if (token->token_type == tt_assign) {
        token_next();
        expr_info = parse_expr_or_call();
        sem_assign(ident, expr_info);
    } else if (token->token_type == tt_left_paren) {
        expr_info = parse_call(ident); // CALL
        sem_dispose_value_from_call(expr_info);
    } else {
        unexpected_token(token);
    }
}

ExprInfo *parse_call(Token *func_name) {
    // CALL = tt_left_paren ARGUMENT_LIST tt_right_paren
    StmtInfo func_info = sem_func_call_name(func_name);
    expect_token(tt_left_paren);
    token_next();
    unsigned nr_args = parse_argument_list(func_info);
    Token *closing_paren = expect_token(tt_right_paren);
    token_next();
    return sem_func_call_end(closing_paren, func_info, nr_args);
}

bool parse_stmt_if() {
    // STMT_IF = tt_kw_if
    // CONDITION
    // tt_left_curly STMT_LIST tt_right_curly
    // tt_kw_else
    // tt_left_curly STMT_LIST tt_right_curly
    Token *kw_if = token_peek();
    token_next();
    StmtInfo stmt_info = parse_if_condition(kw_if); // CONDITION

    expect_token(tt_left_curly);
    token_next();
    bool contains_ret = parse_stmt_list(); // STMT_LIST
    sem_if_then(stmt_info);
    expect_token(tt_right_curly);
    token_next();

    expect_token(tt_kw_else); // tt_kw_else
    token_next();

    expect_token(tt_left_curly);
    token_next();
    contains_ret &= parse_stmt_list(); // STMT_LIST
    sem_if_else(stmt_info);
    expect_token(tt_right_curly);
    token_next();

    return contains_ret;
}

StmtInfo parse_if_condition(Token *keyword) {
    // CONDITION = EXPR
    // CONDITION = tt_kw_let tt_ident
    Token *token = token_peek();
    StmtInfo stmt_info;

    if (token->token_type == tt_kw_let) {
        token_next();
        Token *optional_var = expect_token(tt_ident);
        stmt_info = sem_if_let(optional_var);
        token_next();
    } else {
        ExprInfo *condition = parse_expr();
        stmt_info = sem_if_condition(keyword, condition);
    }

    return stmt_info;
}

void parse_stmt_while() {
    // STMT_WHILE = tt_kw_while
    // CONDITION
    // tt_left_curly STMT_LIST tt_right_curly
    Token *kw_while = token_peek();
    token_next();
    StmtInfo stmt_info = sem_while_begin();
    ExprInfo *condition = parse_expr();
    sem_while_condition(stmt_info, kw_while, condition);

    expect_token(tt_left_curly);
    token_next();
    parse_stmt_list(); // STMT_LIST
    sem_while_end(stmt_info);
    expect_token(tt_right_curly);
    token_next();
}

void parse_stmt_return() {
    // STMT_RETURN = tt_kw_return EXPR_OPT
    Token *kw_return = expect_token(tt_kw_return);
    token_next();
    ExprInfo *expr_return = parse_expr_opt();
    sem_stmt_return(kw_return, expr_return);
}

void parse_param_list(bool call_sem_on_header) {
    // PARAM_LIST = EPSILON
    // PARAM_LIST = PARAM PARAM_REST

    // PARAM = OPT_IDENT OPT_IDENT tt_colon TYPE

    // OPT_IDENT = ident
    // OPT_IDENT = tt_underscore

    Token *token = token_peek();
    switch (token->token_type) {
    case tt_ident: // PARAM
    case tt_underscore:
        parse_param(call_sem_on_header);
        parse_param_rest(call_sem_on_header);
        break;

    default: // EPSILON
        return;
    }
}

void parse_param(bool call_sem_on_header) {
    // PARAM = OPT_IDENT OPT_IDENT tt_colon TYPE

    Token *param_name = parse_opt_ident();  // OPT_IDENT
    Token *param_ident = parse_opt_ident(); // OPT_IDENT
    expect_token(tt_colon);                 // tt_colon
    token_next();
    Token *param_type = parse_type(); // TYPE
    if (call_sem_on_header) {
        sem_func_def_param(param_name, param_ident, param_type);
    }
}

Token *parse_opt_ident() {
    // OPT_IDENT = ident
    // OPT_IDENT = tt_underscore

    Token *token = token_peek();
    if ((token->token_type == tt_ident) ||
        (token->token_type == tt_underscore)) {
        token_next();
    } else {
        unexpected_token(token);
    }
    return token;
}

void parse_param_rest(bool call_sem_on_header) {
    // PARAM_REST = tt_comma PARAM PARAM_REST
    // PARAM_REST = EPSILON

    Token *token = token_peek();
    if (token->token_type == tt_comma) {
        token_next();
        parse_param(call_sem_on_header);      // PARAM
        parse_param_rest(call_sem_on_header); // PARAM_REST
    } else {
        return; // EPSILON
    }
}

// `out_type_token` is an out parameter that is set to the
// token containing the type of declared variable (if there is none,
// nothing will be set)
ExprInfo *parse_decl(Token **out_type_token) {
    // DECL = tt_colon TYPE DEF
    // DECL = tt_assign EXPR_OR_CALL

    Token *token = token_peek();
    if (token->token_type == tt_colon) {
        token_next();
        *out_type_token = parse_type();
        return parse_def();
    } else if (token->token_type == tt_assign) {
        token_next();
        return parse_expr_or_call();
    } else {
        unexpected_token(token);
    }
}

Token *parse_type() {
    // TYPE = tt_kw_Double
    // TYPE = tt_kw_Double_opt ; Double?
    // TYPE = tt_kw_Int
    // TYPE = tt_kw_Int_opt  ; Int?
    // TYPE = tt_kw_String
    // TYPE = tt_kw_String_opt ; String?
    // TYPE = tt_kw_Bool
    // TYPE = tt_kw_Bool_opt ; Bool?

    Token *token = token_peek();
    switch (token->token_type) {
    case tt_kw_Double:
    case tt_kw_Double_opt:
    case tt_kw_Int:
    case tt_kw_Int_opt:
    case tt_kw_String:
    case tt_kw_String_opt:
    case tt_kw_Bool:
    case tt_kw_Bool_opt:
        token_next();
        return token;

    default:
        unexpected_token(token);
    }
}

// returns
ExprInfo *parse_def() {
    // DEF = tt_assign EXPR_OR_CALL
    // DEF = EPSILON

    Token *token = token_peek();
    if (token->token_type == tt_assign) {
        token_next();
        return parse_expr_or_call();
    } else {
        return NULL; // EPSILON
    }
}

/// returns number of parsed arguments
unsigned parse_argument_list(StmtInfo func_info) {
    // ARGUMENT_LIST = EPSILON
    // ARGUMENT_LIST = ARGUMENT ARGUMENT_REST

    Token *token = token_peek();
    switch (token->token_type) {
    case tt_ident:
    case tt_lit_Int:
    case tt_lit_Double:
    case tt_lit_String:
    case tt_lit_Bool:
    case tt_kw_nil:
        parse_argument(func_info, 0);             // ARGUMENT
        return parse_argument_rest(func_info, 1); // ARGUMENT_REST

    default:
        break;
    }

    return 0;
}

void parse_argument(StmtInfo func_info, unsigned arg_position) {
    // ARGUMENT = ident OPT_TERM
    // ARGUMENT = LITERAL

    Token *first_token = token_peek();
    Token *second_token = NULL;
    switch (first_token->token_type) {
    case tt_ident: // ident
        token_next();
        second_token = parse_opt_term();
        break;

    case tt_lit_Int: // LITERAL
    case tt_lit_Double:
    case tt_lit_String:
    case tt_lit_Bool:
    case tt_kw_nil:
        parse_literal();
        break;

    default:
        unexpected_token(first_token);
    }

    if (second_token == NULL) {
        // the argument is unnamed -> first token is term
        sem_func_call_arg(func_info, NULL, first_token, arg_position);
    } else {
        // parameter is named -> first token is name
        sem_func_call_arg(func_info, first_token, second_token, arg_position);
    }
}

/// returns number of parsed arguments
unsigned parse_argument_rest(StmtInfo func_info, unsigned arg_position) {
    // ARGUMENT_REST = tt_comma ARGUMENT ARGUMENT_REST
    // ARGUMENT_REST = EPSILON

    Token *token = token_peek();
    if (token->token_type == tt_comma) {
        token_next();
        parse_argument(func_info, arg_position);
        return parse_argument_rest(func_info, arg_position + 1);
    } else {
        // position is counted from zero => position 1 means we were supposed to
        // parse second argument => total nr of arguments is one
        return arg_position;
    }
}

Token *parse_literal() {
    // LITERAL = tt_lit_Int
    // LITERAL = tt_lit_Double
    // LITERAL = tt_lit_String
    // LITERAL = tt_lit_Bool

    Token *token = token_peek();
    switch (token->token_type) {
    case tt_lit_Int:
    case tt_lit_Double:
    case tt_lit_String:
    case tt_lit_Bool:
    case tt_kw_nil:
        token_next();
        return token;

    default:
        unexpected_token(token);
    }
}

Token *parse_term() {
    // TERM = ident
    // TERM = LITERAL
    Token *token = token_peek();
    switch (token->token_type) {
    case tt_ident: // ident
        token_next();
        break;

    case tt_lit_Int: // LITERAL
    case tt_lit_Double:
    case tt_lit_String:
    case tt_lit_Bool:
    case tt_kw_nil:
        parse_literal();
        break;

    default:
        unexpected_token(token);
    }
    return token;
}

// returns valid ptr if term was parsed, NULL otherwise
Token *parse_opt_term() {
    // OPT_TERM = EPSILON
    // OPT_TERM = tt_colon TERM
    Token *token = token_peek();
    if (token->token_type == tt_colon) {
        token_next();
        return parse_term();
    } else {
        return NULL; // EPSILON
    }
}

Token *parse_return_type() {
    // RETURN_TYPE = EPSILON
    // RETURN_TYPE = tt_arrow TYPE
    // funkce vrátí Int
    Token *token = token_peek();
    if (token->token_type == tt_arrow) // tt_arrow
    {
        token_next();
        return parse_type();
    } else {
        return NULL; // EPSILON
    }
}
