/*
 * sem_common.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef SEM_COMMON_H
#define SEM_COMMON_H

//! contains common code used by the whole backend,
//! mostly just helper functions for handling types

#include "global_include.h"

typedef enum {
    type_bool,
    type_bool_opt,
    type_int,
    type_int_opt,
    type_double,
    type_double_opt,
    type_string,
    type_string_opt,
} Type;

/// initializes all data structures of semantic analysis and codegen
void sem_init();

/// prints error and exits the program with internal error exit code
void error_print_exit_internal(char *err_msg);

/// generates a globally unique id
unsigned sem_id_get();

/// converts token to Type
Type sem_type_get(Token *token);

/// returns true is type is optional
bool sem_type_is_opt(Type type);

/// returns true if lhs is optional type of rhs
bool sem_type_is_option_of(Type lhs, Type rhs);

/// returns unwrapped type of optional `type`
Type sem_type_unwrap(Type type);

/// deallocates all data structures of semantic analysis and codegen
void sem_destroy();

#endif // SEM_COMMON_H
