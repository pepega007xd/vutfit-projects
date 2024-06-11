/*
 * sem_expr.c
 *
 * xvehov01 Jaroslav Vehovský
 * xbrabl04 Tomáš Brablec
 *
 */

#include "sem_expr.h"
#include "codegen.h"
#include "global_include.h"
#include "sem_common.h"
#include "sem_expr_impl.h"
#include "sem_state.h"

#define DEF_EXPR_INFO()                                                        \
    ExprInfo *expr_info = sgc_malloc(sizeof(ExprInfo));                        \
    expr_info->is_castable = false;                                            \
    expr_info->is_unknown = false;                                             \
    expr_info->is_invalid = false;

#define FREE_RETURN()                                                          \
    sgc_free(lhs);                                                             \
    sgc_free(rhs);                                                             \
    return expr_info;

ExprInfo *sem_term(Token *term) {
    DEF_EXPR_INFO();

    VarInfo *var_info;

    codegen_append("PUSHS ");
    switch (term->token_type) {

    case tt_ident:
        var_info = sem_var_lookup(term);
        if (var_info == NULL)
            error_print_exit(term, error_undefined_var, "Undefined variable.");

        if (!var_info->is_initialized)
            error_print_exit(term, error_undefined_var,
                             "Uninitialized variable.");

        expr_info->type = var_info->type;
        codegen_append_var(var_info);

        break;

    case tt_lit_Int:
        codegen_append_int(term->content.int_lit);
        expr_info->is_castable = true;
        expr_info->type = type_int;
        break;

    case tt_lit_Double:
        codegen_append_double(term->content.double_lit);
        expr_info->type = type_double;
        break;

    case tt_lit_String:
        codegen_append_string(term->content.string_lit);
        expr_info->type = type_string;
        break;

    case tt_lit_Bool:
        codegen_append_bool(term->content.bool_lit);
        expr_info->type = type_bool;
        break;

    case tt_kw_nil:
        codegen_append_nil();
        expr_info->is_unknown = true;
        break;

    default:
        error_print_exit(term, error_internal,
                         "Non term token passed to sem_term");
    }

    codegen_append_newl("");
    return expr_info;
}

/// fails if any operand has unknown type (nil)
void check_unknown_type(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    if (lhs->is_unknown || rhs->is_unknown) {
        error_print_exit(op, error_type, "Cannot pass nil into this operator.");
    }
}

/// fails if any operand has boolean type
void check_bool_type(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    if (lhs->type == type_bool || rhs->type == type_bool) {
        error_print_exit(op, error_type,
                         "Cannot pass boolean operand into this operator.");
    }
}

// casts second-to-top operand on stack from int to double
void cast_lhs() {
    codegen_append_newl("POPS GF@$rhs");
    codegen_append_newl("POPS GF@$lhs");

    codegen_append_newl("INT2FLOAT GF@$tmp GF@$lhs");

    codegen_append_newl("PUSHS GF@$tmp");
    codegen_append_newl("PUSHS GF@$rhs");
}

// casts top operand on stack from int to double
void cast_rhs() {
    codegen_append_newl("POPS GF@$rhs");
    codegen_append_newl("INT2FLOAT GF@$tmp GF@$rhs");
    codegen_append_newl("PUSHS GF@$tmp");
}

Type arith_type_check_convert(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    // check for 'nil' value
    check_unknown_type(op, lhs, rhs);

    // check for optional types
    if (sem_type_is_opt(lhs->type) || sem_type_is_opt(rhs->type)) {
        error_print_exit(op, error_type,
                         "Cannot pass nullable operand into this operator.");
    }

    // check for boolean
    check_bool_type(op, lhs, rhs);

    // operation on two ints, doubles, or strings is OK
    if (lhs->type == rhs->type) {
        return lhs->type;
    }

    // cast lhs to double
    if (lhs->type == type_int && rhs->type == type_double && lhs->is_castable) {
        cast_lhs();
        return type_double;
    }

    // cast rhs to double
    if (lhs->type == type_double && rhs->type == type_int && rhs->is_castable) {
        cast_rhs();
        return type_double;
    }

    // if this is reached, some other invalid combination of input types
    // is present, eg (double, string)
    error_print_exit(op, error_type, "Wrong operand types for this operator.");
}

ExprInfo *sem_plus(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    DEF_EXPR_INFO();
    Type type = arith_type_check_convert(op, lhs, rhs);

    if (type == type_int || type == type_double) {
        codegen_append_newl("ADDS");
    } else {
        // concat instruction doesn't have a stack version
        codegen_append_newl("POPS GF@$rhs");
        codegen_append_newl("POPS GF@$lhs");

        codegen_append_newl("CONCAT GF@$tmp GF@$lhs GF@$rhs");

        codegen_append_newl("PUSHS GF@$tmp");
    }

    expr_info->type = type;
    FREE_RETURN();
}

ExprInfo *sem_minus(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    DEF_EXPR_INFO();

    Type type = arith_type_check_convert(op, lhs, rhs);
    if (type == type_int || type == type_double) {
        codegen_append_newl("SUBS");
    } else {
        error_print_exit(op, error_type,
                         "Cannot apply minus operator to this type.");
    }

    expr_info->type = type;
    FREE_RETURN();
}

ExprInfo *sem_times(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    DEF_EXPR_INFO();

    Type type = arith_type_check_convert(op, lhs, rhs);
    if (type == type_int || type == type_double) {
        codegen_append_newl("MULS");
    } else {
        error_print_exit(op, error_type,
                         "Cannot apply times operator to this type.");
    }

    expr_info->type = type;
    FREE_RETURN();
}

ExprInfo *sem_divide(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    DEF_EXPR_INFO();

    Type type = arith_type_check_convert(op, lhs, rhs);

    // integer division
    if (type == type_int) {
        codegen_append_newl("IDIVS");
    }
    // floating point division
    else if (type == type_double) {
        codegen_append_newl("DIVS");
    } else {
        error_print_exit(op, error_type, "Wrong operand types for division.");
    }

    expr_info->type = type;
    FREE_RETURN();
}

ExprInfo *sem_option(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    DEF_EXPR_INFO();

    if (rhs->is_unknown) {
        error_print_exit(op, error_type,
                         "Right hand side of option operator cannot be nil.");
    }

    // let a: Double?
    // a ?? 5
    if (lhs->type == type_double_opt && rhs->type == type_int &&
        rhs->is_castable) {
        cast_rhs();
        rhs->type = type_double;
    }

    // lhs must be the optional type of rhs, or "unknown" type (nil keyword)
    if ((lhs->is_unknown && !sem_type_is_opt(rhs->type)) ||
        sem_type_is_option_of(lhs->type, rhs->type)) {

        unsigned label_take_rhs = sem_id_get();
        unsigned label_end = sem_id_get();

        codegen_append_newl("POPS GF@$rhs");
        codegen_append_newl("POPS GF@$lhs");

        codegen_append_newl("TYPE GF@$tmp GF@$lhs");

        // if type is nil, jump to code that takes rhs
        codegen_append_newl(
            "JUMPIFEQ $sem_option_take_rhs$%d GF@$tmp string@nil",
            label_take_rhs);

        // taking lhs
        codegen_append_newl("PUSHS GF@$lhs");
        codegen_append_newl("JUMP $sem_option_end$%d", label_end);

        // taking rhs
        codegen_append_newl("LABEL $sem_option_take_rhs$%d", label_take_rhs);
        codegen_append_newl("PUSHS GF@$rhs");

        codegen_append_newl("LABEL $sem_option_end$%d", label_end);
    } else {
        error_print_exit(op, error_type, "Wrong types for option operator.");
    }

    expr_info->type = rhs->type;
    FREE_RETURN();
}

ExprInfo *sem_yolo(Token *op, ExprInfo *operand) {
    DEF_EXPR_INFO();

    if (operand->is_unknown)
        error_print_exit(op, error_type,
                         "Cannot pass nil into unwrap operator.");

    if (sem_type_is_opt(operand->type)) {
        expr_info->type = sem_type_unwrap(operand->type);
    } else {
        error_print_exit(
            op, error_type,
            "Only optional types can be passed into unwrap operator.");
    }

    sgc_free(operand);
    return expr_info;
}

// type checking / conversion for equality comparison operators (==, !=)
void eq_type_check_convert(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    // `nil` can be compared to anything in == and !=
    if (lhs->is_unknown || rhs->is_unknown) {
        return;
    }

    // check for optional type on one side and non-optional on other side
    if (sem_type_is_option_of(lhs->type, rhs->type) ||
        sem_type_is_option_of(rhs->type, lhs->type)) {
        return;
    }

    // operation on two ints, doubles, or strings is OK
    if (lhs->type == rhs->type) {
        return;
    }

    // cast lhs to double
    if (lhs->type == type_int &&
        (rhs->type == type_double || rhs->type == type_double_opt) &&
        lhs->is_castable) {
        cast_lhs();
        return;
    }

    // cast rhs to double
    if ((lhs->type == type_double || lhs->type == type_double_opt) &&
        rhs->type == type_int && rhs->is_castable) {
        cast_rhs();
        return;
    }

    // if this is reached, some other invalid combination of input types
    // is present, eg (double, string)
    error_print_exit(op, error_type, "Wrong operand types for this operator.");
}

// type checking / conversion for ordering comparison operators (>, <=, ...)
void ord_type_check_convert(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    // check for 'nil' value
    check_unknown_type(op, lhs, rhs);

    // check for optional types
    if (sem_type_is_opt(lhs->type) || sem_type_is_opt(rhs->type)) {
        error_print_exit(op, error_type,
                         "Cannot pass nullable operand into this operator.");
    }

    // operation on two ints, doubles, or strings is OK
    if (lhs->type == rhs->type) {
        return;
    }

    // cast lhs to double
    if (lhs->type == type_int && rhs->type == type_double && lhs->is_castable) {
        cast_lhs();
        return;
    }

    // cast rhs to double
    if (lhs->type == type_double && rhs->type == type_int && rhs->is_castable) {
        cast_rhs();
        return;
    }

    // if this is reached, some other invalid combination of input types
    // is present, eg (double, string)
    error_print_exit(op, error_type, "Wrong operand types for this operator.");
}

ExprInfo *sem_equals(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    DEF_EXPR_INFO();
    eq_type_check_convert(op, lhs, rhs);

    codegen_append_newl("EQS");

    expr_info->type = type_bool;
    FREE_RETURN();
}

ExprInfo *sem_not_equals(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    DEF_EXPR_INFO();
    eq_type_check_convert(op, lhs, rhs);

    codegen_append_newl("EQS");
    codegen_append_newl("NOTS");

    expr_info->type = type_bool;
    FREE_RETURN();
}

ExprInfo *sem_less_than(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    DEF_EXPR_INFO();
    ord_type_check_convert(op, lhs, rhs);

    codegen_append_newl("LTS");

    expr_info->type = type_bool;
    FREE_RETURN();
}

ExprInfo *sem_greater_than(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    DEF_EXPR_INFO();
    ord_type_check_convert(op, lhs, rhs);

    codegen_append_newl("GTS");

    expr_info->type = type_bool;
    FREE_RETURN();
}

ExprInfo *sem_less_equals(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    DEF_EXPR_INFO();
    ord_type_check_convert(op, lhs, rhs);

    codegen_append_newl("GTS");
    codegen_append_newl("NOTS");

    expr_info->type = type_bool;
    FREE_RETURN();
}

ExprInfo *sem_greater_equals(Token *op, ExprInfo *lhs, ExprInfo *rhs) {
    DEF_EXPR_INFO();
    ord_type_check_convert(op, lhs, rhs);

    codegen_append_newl("LTS");
    codegen_append_newl("NOTS");

    expr_info->type = type_bool;
    FREE_RETURN();
}
