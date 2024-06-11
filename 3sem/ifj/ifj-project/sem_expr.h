/*
 * sem_expr.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef SEM_EXPR_H
#define SEM_EXPR_H

#include "global_include.h"

// opaque type, defined in sem_expr.c
typedef struct ExprInfo ExprInfo;

/// semantic analysis of term (literal or identifier)
ExprInfo *sem_term(Token *lit);

/// semantic analyses of operators (op is token for operator)

ExprInfo *sem_plus(Token *op, ExprInfo *lhs, ExprInfo *rhs);
ExprInfo *sem_minus(Token *op, ExprInfo *lhs, ExprInfo *rhs);
ExprInfo *sem_times(Token *op, ExprInfo *lhs, ExprInfo *rhs);
ExprInfo *sem_divide(Token *op, ExprInfo *lhs, ExprInfo *rhs);
ExprInfo *sem_option(Token *op, ExprInfo *lhs, ExprInfo *rhs);
ExprInfo *sem_yolo(Token *op, ExprInfo *operand);
ExprInfo *sem_equals(Token *op, ExprInfo *lhs, ExprInfo *rhs);
ExprInfo *sem_not_equals(Token *op, ExprInfo *lhs, ExprInfo *rhs);
ExprInfo *sem_less_than(Token *op, ExprInfo *lhs, ExprInfo *rhs);
ExprInfo *sem_greater_than(Token *op, ExprInfo *lhs, ExprInfo *rhs);
ExprInfo *sem_less_equals(Token *op, ExprInfo *lhs, ExprInfo *rhs);
ExprInfo *sem_greater_equals(Token *op, ExprInfo *lhs, ExprInfo *rhs);

#endif // SEM_EXPR_H
