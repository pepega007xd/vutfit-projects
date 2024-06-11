/*
 * sem_stmt.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef SEM_STMT_H
#define SEM_STMT_H

#include "function_table.h"
#include "global_include.h"
#include "sem_expr.h"

typedef struct {
        unsigned if_end_label_id;
        unsigned else_label_id;
        unsigned while_condition_label_id;
        unsigned while_end_label_id;
        FunctionSig *called_function;
        bool calling_write_func;
} StmtInfo;

/// these functions do the semantic analysis and codegen for statments

/// `ident` is the token of variable name identifier,
/// `type` is the token of type specifier (pass NULL if there is none),
/// `value` is the result of running precedence parser on the initializer
/// expression (pass NULL if there is none)
void sem_let(Token *ident, Token *type, ExprInfo *value);

/// `ident` is the token of variable name identifier,
/// `type` is the token of type specifier (pass NULL if there is none),
/// `value` is the result of running precedence parser on the initializer
/// expression (pass NULL if there is none)
void sem_var(Token *ident, Token *type, ExprInfo *value);

/// `ident` is the identifier token of the variable being assigned to,
/// `value` is the result of evaluating the assigned expression
void sem_assign(Token *ident, ExprInfo *value);

/// call this right after parsing the condition,
/// `if_kw` is the token of the keyword `if` (used for printing errors)
/// `condition` is the result of parsing the condition expression,
/// the return value must be passed into sem_if_then and sem_if_else
StmtInfo sem_if_condition(Token *if_kw, ExprInfo *condition);

/// call this right after parsing the variable identifier after `if let`,
/// and pass it as the `ident` argument, then call the same functions as for
/// normal if statement
StmtInfo sem_if_let(Token *ident);

/// call this right after parsing the "then" code block - the code block
/// that gets executed if the condition is true
void sem_if_then(StmtInfo stmt_info);

/// call this right after parsing the "else" code block - the code block
/// that gets executed if the condition is false
void sem_if_else(StmtInfo stmt_info);

/// call this right after parsing the `while` keyword,
/// *before* parsing the expression,
/// the value returned must be passed into sem_while_condition and sem_while_end
StmtInfo sem_while_begin();

/// call this right after parsing the condition,
/// `while_kw` is the token of the keyword `while` (used for printing errors)
/// `condition` is the result of parsing the condition expression,
/// the return value must be passed into sem_while_end
void sem_while_condition(StmtInfo stmt_info, Token *while_kw,
                         ExprInfo *condition);

/// call this right after parsing the while stmt body
void sem_while_end(StmtInfo stmt_info);

/// call this after parsing return statement,
/// `return_kw` is the token of keyword `return` (used for printing errors)
/// `value` is the result of parsing the expression being returned (pass NULL if
/// there is none)
void sem_stmt_return(Token *return_kw, ExprInfo *value);

/// call this after parsing the function name,
/// `func_name` is the token with function name
void sem_func_def_begin(Token *func_name);

/// call this right after parsing the function body,
/// `func_name` is the token of function name,
/// `contains_ret` is the result of "return analysis"
void sem_func_def_end(Token *func_name, bool contains_ret);

/// call this right after parsing function call name,
/// `name` is the name token of the called function
/// retured StmtInfo must be passed into sem_func_call_arg and sem_func_def_end
StmtInfo sem_func_call_name(Token *name);

/// call this right after parsing each argument,
/// `func_name` is the name of the function we are calling
/// `arg_name` is the name of the argument (pass NULL if there is none)
/// `term` is the term of the argument (either identifer or literal)
/// `arg_position` the position number of the argument (first argument has
/// position 0, second has position 1, etc)
void sem_func_call_arg(StmtInfo func_info, Token *arg_name, Token *term,
                       unsigned arg_position);

/// call this right after parsing the end of function call,
/// returns info about returned value, or NULL if the function is void
/// `closing_paren` is the token of closing parenthesis that ends
///     the func call (for printing errors)
/// `nr_args` is the number of arguments passed into function
ExprInfo *sem_func_call_end(Token *closing_paren, StmtInfo func_info,
                            unsigned nr_args);

/// call this in case function that returns a value is called,
/// and the resulted value is not assigned to anything,
/// `expr_info` is the result of the function call
void sem_dispose_value_from_call(ExprInfo *expr_info);

#endif // SEM_STMT_H
