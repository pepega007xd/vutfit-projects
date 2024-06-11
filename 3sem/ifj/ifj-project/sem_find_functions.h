/*
 * sem_find_functions.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef SEM_FIND_FUNCTIONS_H
#define SEM_FIND_FUNCTIONS_H

#include "global_include.h"

/// finds all functions in token table, parses them using
/// parse_stmt_func_def, and puts information about their headers
/// into function table, then rewinds token table back to start
void find_functions();

/// call this right after parsing the function name,
/// `func_name` is the identifier token of the function's name
void sem_func_def_name(Token *func_name);

/// call this right after parsing a parameter,
/// `name` is the name token of the parameter (pass the underscore token too)
/// `ident` is the identifier token of the parameter (pass the underscore token
/// too) `type` is the type token of the parameter
void sem_func_def_param(Token *name, Token *ident, Token *type);

/// call this right after parsing the function header,
/// `type` is the return type of the function (pass NULL if there is none)
void sem_func_def_return_type(Token *type);

#endif // SEM_FIND_FUNCTIONS_H
