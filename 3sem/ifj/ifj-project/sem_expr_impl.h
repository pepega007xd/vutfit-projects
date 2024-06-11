/*
 * sem_expr_impl.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef SEM_EXPR_IMPL_H
#define SEM_EXPR_IMPL_H

#include "global_include.h"
#include "sem_common.h"

// definition of opaque struct from sem_expr.h
struct ExprInfo {
        Type type;
        bool is_castable; // integer literal can be casted to double
        bool is_unknown;  // nil expression can have any optional type
        bool is_invalid;  // returned from calling a void function
};

#endif // SEM_EXPR_IMPL_H
