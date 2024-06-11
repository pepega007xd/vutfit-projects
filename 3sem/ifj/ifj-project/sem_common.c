/*
 * sem_common.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "sem_common.h"
#include "codegen.h"
#include "function_table.h"
#include "global_include.h"
#include "sem_state.h"

void sem_init() {
    fntab_init();
    sem_state_init();
    codegen_init();
}

unsigned sem_id_get() {
    static unsigned count; // initialized to zero at start of program
    return count++;
}

_Noreturn void error_print_exit_internal(char *err_msg) {
    fprintf(stderr, "%s\n", err_msg);
    sgc_free_all();
    exit(error_internal);
}

Type sem_type_get(Token *token) {
    switch (token->token_type) {
    case tt_kw_Int:
        return type_int;
    case tt_kw_Int_opt:
        return type_int_opt;
    case tt_kw_Double:
        return type_double;
    case tt_kw_Double_opt:
        return type_double_opt;
    case tt_kw_String:
        return type_string;
    case tt_kw_String_opt:
        return type_string_opt;
    case tt_kw_Bool:
        return type_bool;
    case tt_kw_Bool_opt:
        return type_bool_opt;
    default:
        error_print_exit(token, error_internal,
                         "Not a type literal token passed into sem_type_get");
    }
    return 0; // unreachable
}

bool sem_type_is_opt(Type type) {
    return type == type_int_opt || type == type_double_opt ||
           type == type_string_opt || type == type_bool_opt;
}

/// returns true iff lhs is an optional type and rhs is a non-optional variant
/// of that type
bool sem_type_is_option_of(Type lhs, Type rhs) {
    return (lhs == type_int_opt && rhs == type_int) ||
           (lhs == type_double_opt && rhs == type_double) ||
           (lhs == type_string_opt && rhs == type_string) ||
           (lhs == type_double_opt && rhs == type_double);
}

Type sem_type_unwrap(Type type) {
    switch (type) {
    case type_int_opt:
        return type_int;
    case type_double_opt:
        return type_double;
    case type_string_opt:
        return type_string;
    case type_bool_opt:
        return type_bool;
    default:
        error_print_exit_internal(
            "Non optional type passed into sem_type_from_opt");
    }
}

void sem_destroy() {
    fntab_destroy();
    void sem_state_destroy();
    codegen_destroy();
}
