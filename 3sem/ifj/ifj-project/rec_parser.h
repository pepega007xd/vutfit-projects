/*
 * rec_parser.h
 *
 * xkralo07 Anna Králová
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef REC_PARSER_H
#define REC_PARSER_H

#include "sem_expr.h"
#include "sem_stmt.h"

/// the following functions parse their corresponding rules from ll_grammar.ini

ExprInfo *parse_expr();
ExprInfo *parse_expr_opt();
ExprInfo *parse_expr_or_call();
void parse_file();
bool parse_stmt_list();
bool parse_stmt();
void parse_stmt_func_def(bool call_sem_on_header);
void parse_stmt_let();
void parse_stmt_var();
void parse_stmt_ident(Token *ident);
ExprInfo *parse_call(Token *func_name);
bool parse_stmt_if();
StmtInfo parse_if_condition(Token *keyword);
void parse_stmt_while();
void parse_stmt_return();
void parse_param_list(bool call_sem_on_header);
void parse_param(bool call_sem_on_header);
Token *parse_opt_ident();
void parse_param_rest(bool call_sem_on_header);
ExprInfo *parse_decl(Token **out_type_token);
Token *parse_type();
ExprInfo *parse_def();
unsigned parse_argument_list(StmtInfo func_info);
void parse_argument(StmtInfo func_info, unsigned arg_position);
unsigned parse_argument_rest(StmtInfo func_info, unsigned arg_position);
Token *parse_literal();
Token *parse_term();
Token *parse_opt_term();
Token *parse_return_type();
#endif
