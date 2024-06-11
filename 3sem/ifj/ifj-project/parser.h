/*
 * parser.h
 *
 * xvehov01 Jaroslav Vehovský
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef IFJ_PARSER_H
#define IFJ_PARSER_H

#include "global_include.h"
#include "sem_expr.h"

#define STACK_SIZE 100
#define PRECEDENCE_TABLE_SIZE 9

typedef struct expressionStackItem {
        Token *token;
        ExprInfo *exprInfo;
        char flag;
        int is_expression;
        int is_dollar;
} *StackItem;

typedef struct expressionStack {
        StackItem *items;
        int top;
        int size;
} *Stack;

typedef enum {
    R, // reduce
    S, // shift
    E, // error
    X  // marks end of expression
} PrecedenceTable;

/*
 * Function for parsing expression.
 */
ExprInfo *parse_expr();

/*
 * get_precedence_index returns index of token in precedence table.
 */
int get_precedence_index(Token *token);

/*
 * get_precedence returns value from precedence table.
 */
PrecedenceTable get_precedence(StackItem stackItem, StackItem token);

/*
 * reduce reduces expression.
 */
void reduce(Stack stack);

/*
 * is_identifier_or_literal returns 1 if token is identifier or literal,
 * otherwise 0
 */
int is_identifier_or_literal(Token *token);

/*
 * is_infix_operator returns 1 if token is infix operator, otherwise 0
 */
int is_infix_operator(Token *token);

// Stack functions
void stack_init(Stack *stack, int size);
void stack_push(Stack stack, StackItem item);
StackItem stack_top(Stack stack);
StackItem stack_pop(Stack stack);
void stack_destroy(Stack *stack);

#endif // IFJ_PARSER_H
