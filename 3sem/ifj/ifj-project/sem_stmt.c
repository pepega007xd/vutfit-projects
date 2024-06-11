/*
 * sem_stmt.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "sem_stmt.h"
#include "codegen.h"
#include "function_table.h"
#include "global_include.h"
#include "sem_common.h"
#include "sem_expr_impl.h"
#include "sem_state.h"
#include "sgc.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

extern SemState sem_state;

Token *called_func_name;

/// checks whether expected type matches inferred type, casts if needed
void type_check_cast(Token *ident, Type expected, ExprInfo *inferred) {
    if (sem_type_is_opt(expected) && inferred->is_unknown) {
        return; // 'nil' value conforms to any optional type
    }

    if (inferred->is_unknown) {
        error_print_exit(ident, error_type,
                         "Non optional types cannot be initialized with nil.");
    }

    if (expected == inferred->type) {
        return;
    }

    if (sem_type_is_option_of(expected, inferred->type)) {
        return; // we can assign eg. expression of type String to variable of
                // type String?
    }

    // expression result is int literal => can be type casted to double
    if ((expected == type_double || expected == type_double_opt) &&
        inferred->type == type_int && inferred->is_castable) {
        codegen_append_newl("POPS GF@$tmp");
        codegen_append_newl("INT2FLOAT GF@$tmp GF@$tmp");
        codegen_append_newl("PUSHS GF@$tmp");
        return;
    }

    // other invalid combinations of types
    error_print_exit(ident, error_type, "Incompatible with expected type.");
}

void sem_decl(Token *ident, Token *type_token, ExprInfo *value,
              bool is_mutable) {
    Type real_type; // either from token, or inferred, or both

    if (value != NULL && value->is_invalid) { // short-circuit eval
        error_print_exit(
            ident, error_type,
            "Cannot assign the result of calling a void function.");
    }

    if (type_token == NULL) {
        // type is not explicitly provided, we just take the inferred type
        real_type = value->type;
        if (value->is_unknown) {
            error_print_exit(ident, error_type_inference,
                             "Cannot infer type of nil.");
        }
    } else {
        // type is explicitly provided
        real_type = sem_type_get(type_token);
        // if there is initialization, we must check type and optionally
        // typecast
        if (value != NULL) {
            type_check_cast(type_token, real_type, value);
        }
    }

    bool is_initialized = value != NULL || sem_type_is_opt(real_type);
    VarInfo *variable =
        sem_var_add(ident, real_type, is_mutable, is_initialized);
    codegen_append_defvar(variable);

    // if there is initialing value, assign it to variable
    if (value != NULL) {
        codegen_append("POPS ");
        codegen_append_var(variable);
        codegen_append_newl("");
    } else if (sem_type_is_opt(real_type)) {
        // values with optional types are implicitly initialized with nil
        codegen_append("MOVE ");
        codegen_append_var(variable);
        codegen_append(" ");
        codegen_append_nil();
        codegen_append_newl("");
    }

    sgc_free(value);
}

void sem_let(Token *ident, Token *type_token, ExprInfo *value) {
    sem_decl(ident, type_token, value, false);
}

void sem_var(Token *ident, Token *type_token, ExprInfo *value) {
    sem_decl(ident, type_token, value, true);
}

void sem_assign(Token *ident, ExprInfo *value) {
    VarInfo *variable = sem_var_lookup(ident);
    if (variable == NULL) {
        error_print_exit(ident, error_undefined_var, "Undefined variable.");
    }

    // check for initialized immutable variable
    if (!variable->is_mutable && variable->is_initialized) {
        error_print_exit(
            ident, error_other_semantic,
            "Cannot assign to already initialized immutable variable.");
    }

    if (value != NULL && value->is_invalid) { // short-circuit eval
        error_print_exit(
            ident, error_type,
            "Cannot assign the result of calling a void function.");
    }

    variable->is_initialized = true;
    type_check_cast(ident, variable->type, value);

    codegen_append("POPS ");
    codegen_append_var(variable);
    codegen_append_newl("");

    sgc_free(value);
}

StmtInfo sem_if_condition(Token *if_kw, ExprInfo *condition) {
    // generate unique label IDs for labels to jump on
    StmtInfo stmt_info = {.else_label_id = sem_id_get(),
                          .if_end_label_id = sem_id_get()};

    if (condition->is_unknown || condition->type != type_bool) {
        error_print_exit(if_kw, error_type,
                         "Condition of if statement must be of type Bool.");
    }

    // entering new scope, we must tell it to symbol table
    sem_scope_enter();

    // this is useless here, but `if let` requires outer scope for
    // shadowing the new variable, and `sem_scope_leave` `in sem_if_then`
    // must match
    sem_scope_enter();

    // push true to stack, then compare with value from evaluating condition,
    // if the value on stack was a `false`, jump to `else` branch, otherwise
    // continue into the `then` branch
    codegen_append("PUSHS ");
    codegen_append_bool(true);
    codegen_append_newl("");
    codegen_append_newl("JUMPIFNEQS $else$%d", stmt_info.else_label_id);

    sgc_free(condition);
    return stmt_info;
}

StmtInfo sem_if_let(Token *ident) {
    // generate unique label IDs for labels to jump on
    StmtInfo stmt_info = {.else_label_id = sem_id_get(),
                          .if_end_label_id = sem_id_get()};

    VarInfo *variable = sem_var_lookup(ident);
    if (variable == NULL) {
        error_print_exit(ident, error_undefined_var, "Undefined variable");
    }

    // there is no need to check whether variable is initialized,
    // all optional variables are implicitly initalized to nil
    if (!sem_type_is_opt(variable->type)) {
        error_print_exit(
            ident, error_other_semantic,
            "Variable in if let statement must have optional type.");
    }

    // entering new scope, we must tell it to symbol table
    sem_scope_enter();

    codegen_append("TYPE GF@$tmp ");
    codegen_append_var(variable);
    codegen_append_newl("");

    // if variable contained nil, jump to else branch
    codegen_append_newl("JUMPIFEQ $else$%d GF@$tmp string@nil",
                        stmt_info.else_label_id);

    // unwrapped variable is created in `then` scope
    VarInfo *unwrapped_var =
        sem_var_add(ident, sem_type_unwrap(variable->type), false, true);

    // unwrapped variable above must be shadowable =>
    // we need another scope
    sem_scope_enter();

    codegen_append_defvar(unwrapped_var);

    codegen_append("MOVE ");
    codegen_append_var(unwrapped_var);
    // this is the old, nullable var in higher scope
    codegen_append(" ");
    codegen_append_var(variable);
    codegen_append_newl("");

    return stmt_info;
}

void sem_if_then(StmtInfo stmt_info) {
    // inner and outer scopes from `if let`
    sem_scope_leave();
    sem_scope_leave();

    codegen_append_newl("JUMP $if_stmt_end$%d", stmt_info.if_end_label_id);
    codegen_append_newl("LABEL $else$%d", stmt_info.else_label_id);

    sem_scope_enter();
}

void sem_if_else(StmtInfo stmt_info) {
    sem_scope_leave();

    codegen_append_newl("LABEL $if_stmt_end$%d", stmt_info.if_end_label_id);
}

StmtInfo sem_while_begin() {
    StmtInfo stmt_info = {.while_condition_label_id = sem_id_get(),
                          .while_end_label_id = sem_id_get()};

    codegen_append_newl("LABEL $while_condition$%d",
                        stmt_info.while_condition_label_id);

    return stmt_info;
}

void sem_while_condition(StmtInfo stmt_info, Token *while_kw,
                         ExprInfo *condition) {

    if (condition->is_unknown || condition->type != type_bool) {
        error_print_exit(while_kw, error_type,
                         "Condition of while statement must be of type Bool.");
    }

    // entering new scope, we must tell it to symbol table
    sem_scope_enter();

    // push true to stack, then compare with value from evaluating condition,
    // if the value on stack was a `false`, jump to end of while, otherwise
    // continue into the while stmt body
    codegen_append("PUSHS ");
    codegen_append_bool(true);
    codegen_append_newl("");
    codegen_append_newl("JUMPIFNEQS $while_end$%d",
                        stmt_info.while_end_label_id);

    sgc_free(condition);
}

void sem_while_end(StmtInfo stmt_info) {
    codegen_append_newl("JUMP $while_condition$%d",
                        stmt_info.while_condition_label_id);
    codegen_append_newl("LABEL $while_end$%d", stmt_info.while_end_label_id);
    sem_scope_leave();
}

/// checks whether expected type matches inferred type, casts if needed
/// returns error code corresponding to return type
void type_check_cast_for_call(Token *ident, Type expected, ExprInfo *inferred) {
    if (sem_type_is_opt(expected) && inferred->is_unknown) {
        return; // 'nil' value conforms to any optional type
    }

    if (inferred->is_unknown) {
        error_print_exit(ident, error_call,
                         "This value's type must not be nullable.");
    }

    if (expected == inferred->type) {
        return;
    }

    // i'm not sure whether we can consider every non-nullable argument to be
    // compatible with nullable parameter type, but let's hope that yes
    if (sem_type_is_option_of(expected, inferred->type)) {
        return;
    }

    // expression result is int literal => can be type casted to double
    if ((expected == type_double || expected == type_double_opt) &&
        inferred->type == type_int && inferred->is_castable) {
        codegen_append_newl("POPS GF@$tmp");
        codegen_append_newl("INT2FLOAT GF@$tmp GF@$tmp");
        codegen_append_newl("PUSHS GF@$tmp");
        return;
    }

    // other invalid combinations of types
    error_print_exit(ident, error_call,
                     "Wrong type of function argument or return value.");
}

void sem_stmt_return(Token *return_kw, ExprInfo *value) {
    if (!sem_in_local()) {
        // this is weird, but "official"
        // https://discord.com/channels/461541385204400138/753997224144404577/1176523592377319508
        error_print_exit(return_kw, error_parser,
                         "Return statement cannot be outside of a function.");
    }

    FunctionSig *called_func = fntab_lookup(called_func_name->content.ident);

    // check if function is void and expr is returned
    if (called_func->is_void && value != NULL) {
        error_print_exit(return_kw, error_invalid_return,
                         "This function does not return anything, cannot "
                         "return expression.");
    }

    if (!called_func->is_void) {
        // check if function is not void and no expr is returned
        if (value == NULL)
            error_print_exit(return_kw, error_invalid_return,
                             "This function returns a value, you must return "
                             "expression.");

        // if the function actually returns the correct type
        type_check_cast_for_call(return_kw, called_func->return_type, value);
        // redefinition of $return cannot happen since we'll be leaving the
        // function immediately upon hitting this instruction
        codegen_append_newl("DEFVAR LF@$return");
        codegen_append_newl("POPS LF@$return");
    }

    codegen_append_newl("POPFRAME");
    codegen_append_newl("RETURN");

    sgc_free(value);
}

void sem_func_def_begin(Token *func_name) {
    // check for nested function definition
    if (sem_in_local()) {
        error_print_exit(
            func_name, error_other_semantic,
            "Function definition inside another function is not supported.");
    }

    if (sem_scope_depth() != 0) {
        error_print_exit(
            func_name, error_other_semantic,
            "Function definition cannot be inside another statement.");
    }

    sem_function_enter();
    FunctionSig *func = fntab_lookup(func_name->content.ident);

    // we must tell the symbol table that parameter variables
    // are now present in scope
    for (unsigned i = 0; i < func->params.count; i++) {
        FunctionParam param = func->params.array[i];
        if (param.ident != NULL) {
            VarInfo *ident_var =
                sem_var_add(param.ident, param.type, false, true);

            // we must change the id to match the parameter's id
            ident_var->id = param.ident_id;
        }
    }

    // enter another scope, parameters can be shadowed in function's top level,
    // therefore we need to put parameters in their own scope
    sem_scope_enter();

    // codegen creates defintions and code buffers
    codegen_enter_func(func_name);
}

void sem_func_def_end(Token *func_name, bool contains_ret) {
    FunctionSig *func = fntab_lookup(func_name->content.ident);

    if (!func->is_void && !contains_ret) {
        error_print_exit(func->name, error_other_semantic,
                         "This function returns a value, but return wasn't "
                         "found on all control paths.");
    }

    codegen_append_newl("POPFRAME");
    codegen_append_newl("RETURN");

    // codegen flushes function definitions and code
    codegen_leave_func();

    // left inner scope for function body, now only the only remaining scope is
    // the outer scope for params, which will be left in sem_function_leave
    sem_scope_leave();

    // tell symbol table we're leaving the function body
    sem_function_leave();
}

StmtInfo sem_func_call_name(Token *name) {
    StmtInfo stmt_info;

    // check for function `write`, it is inlined (not defined in function table)
    if (!strcmp(name->content.ident, "write")) {
        stmt_info.calling_write_func = true;
        return stmt_info;
    } else {
        stmt_info.calling_write_func = false;
    }

    FunctionSig *func = fntab_lookup(name->content.ident);

    // check if called function exists
    if (func == NULL)
        error_print_exit(name, error_ident, "Undefined function.");

    stmt_info.called_function = func;

    codegen_append_newl("CREATEFRAME");

    return stmt_info;
}

void sem_func_call_arg(StmtInfo func_info, Token *arg_name, Token *term,
                       unsigned arg_position) {
    // we can reuse the code of sem_term -> this does the type inference
    // and pushes the term to stack
    ExprInfo *term_info = sem_term(term);

    // special case for write function, which is inlined
    if (func_info.calling_write_func) {
        codegen_append_newl("POPS GF@$tmp");
        codegen_append_newl("WRITE GF@$tmp");
        return;
    }

    FunctionSig *func = func_info.called_function;
    FunctionParam *param = fntab_lookup_param(func, arg_position);

    // corresponding function parameter not found
    if (param == NULL)
        error_print_exit(term, error_call, "Too many arguments.");

    if (arg_name != NULL) {
        if (param->name == NULL)
            error_print_exit(
                arg_name, error_other_semantic,
                "This parameter is unnamed in function definition.");

        if (strcmp(arg_name->content.ident, param->name->content.ident))
            error_print_exit(arg_name, error_call, "Wrong parameter name.");
    } else if (param->name != NULL) {
        error_print_exit(term, error_other_semantic,
                         "This parameter is named in function definition.");
    }

    // this does the type check/cast
    type_check_cast_for_call(term, param->type, term_info);

    // parameter's ident is NULL -> it's not used in function -> don't add it
    if (param->ident == NULL) {
        sgc_free(term_info);
        return;
    }

    // redefinition of these variables cannot happen, we're always defining them
    // on a freshly cleaned TF
    if (func->is_builtin) {
        // builtin functions don't have unique IDs, we must not append them
        codegen_append_newl("DEFVAR TF@%s", param->ident->content.ident,
                            param->ident_id);
        codegen_append_newl("POPS TF@%s", param->ident->content.ident,
                            param->ident_id);
    } else {
        codegen_append_newl("DEFVAR TF@%s$%d", param->ident->content.ident,
                            param->ident_id);
        codegen_append_newl("POPS TF@%s$%d", param->ident->content.ident,
                            param->ident_id);
    }

    sgc_free(term_info);
}

ExprInfo *sem_func_call_end(Token *closing_paren, StmtInfo func_info,
                            unsigned nr_args) {
    ExprInfo *expr_info = sgc_malloc(sizeof(ExprInfo));
    expr_info->is_invalid = true;

    // special case - write function is inlined
    if (func_info.calling_write_func) {
        return expr_info;
    }

    FunctionSig *func = func_info.called_function;

    if (func->params.count != nr_args) {
        error_print_exit(closing_paren, error_call,
                         "Wrong number of arguments.");
    }

    codegen_append_newl("CALL %s", func->name->content.ident);
    // after returning from function, the return value will be in TF@$return,
    // if there is any
    if (!func->is_void) {
        codegen_append_newl("PUSHS TF@$return");
        expr_info->type = func->return_type;
        expr_info->is_unknown = false;
        expr_info->is_castable = false;
        expr_info->is_invalid = false;
    }

    return expr_info;
}

void sem_dispose_value_from_call(ExprInfo *expr_info) {
    // function might've been void, the parser doesn't know
    if (!expr_info->is_invalid) {
        // remove unused value from stack
        codegen_append_newl("POPS GF@$tmp");
    }
    sgc_free(expr_info);
}
