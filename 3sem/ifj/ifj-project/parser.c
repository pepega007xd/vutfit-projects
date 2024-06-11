/*
 * parser.c
 *
 * xvehov01 Jaroslav Vehovský
 * xbrabl04 Tomáš Brablec
 *
 */

#include "parser.h"
#include "global_include.h"
#include "sem_expr.h"
#include "token_table.h"

const PrecedenceTable
    precedence_table[PRECEDENCE_TABLE_SIZE][PRECEDENCE_TABLE_SIZE] = {
        // [0]+-    [1]*/   [2]<>  [3]!  [4]??   [5](  [6])   [7]i   [8]$
        {R, S, R, S, R, S, R, S, R}, // 0 +-
        {R, R, R, S, R, S, R, S, R}, // 1 */
        {S, S, E, S, R, S, R, S, R}, // 2 <>
        {R, R, R, E, R, E, R, E, R}, // 3 !
        {S, S, S, S, S, S, R, S, R}, // 4 ??
        {S, S, S, S, S, S, S, S, E}, // 5 (
        {R, R, R, R, R, E, R, E, R}, // 6 )
        {R, R, R, R, R, E, R, E, R}, // 7 i
        {S, S, S, S, S, S, E, S, X}, // 8 $
                                     // R = reduce, S = shift, E = error
};

int get_precedence_index(Token *token) {
    switch (token->token_type) {
    case tt_plus:
    case tt_minus:
        return 0;
    case tt_times:
    case tt_divide:
        return 1;
    case tt_less_than:
    case tt_greater_than:
    case tt_less_equals:
    case tt_greater_equals:
    case tt_equals:
    case tt_not_equals:
        return 2;
    case tt_yolo:
        return 3;
    case tt_option:
        return 4;
    case tt_left_paren:
        return 5;
    case tt_right_paren:
        return 6;
    case tt_ident:
    case tt_lit_Bool:
    case tt_lit_Double:
    case tt_lit_Int:
    case tt_lit_String:
    case tt_kw_nil:
        return 7;
    default:
        // If by any change some other token got here, returns -1 resulting in
        // error
        return -1;
    }
}

PrecedenceTable get_precedence(StackItem stackTop, StackItem newToken) {
    int stackTopIndex;
    int newTokenIndex;

    if (newToken->is_dollar) {
        newTokenIndex = 8;
    } else {
        newTokenIndex = get_precedence_index(newToken->token);
    }

    if (stackTop->is_dollar) {
        stackTopIndex = 8;
    } else {
        stackTopIndex = get_precedence_index(stackTop->token);
    }

    // if token is not in precedence table, return E
    if (stackTopIndex == -1 || newTokenIndex == -1) {
        return E;
    }

    return precedence_table[stackTopIndex][newTokenIndex];
}

ExprInfo *parse_expr() {
    Stack stack;
    stack_init(&stack, STACK_SIZE);
    // Pushes '$' to stack
    StackItem dollar =
        (StackItem)sgc_malloc(sizeof(struct expressionStackItem));
    dollar->is_expression = 0;
    dollar->is_dollar = 1;
    stack_push(stack, dollar);

    int validExp = 1;
    Token *token = token_peek();

    // Checks if expression is valid
    Token *nextToken;
    while (validExp) {
        switch (token->token_type) {
        case tt_ident:
            token_next();
            nextToken = token_peek();
            // If met, no longer valid expression
            if (nextToken->token_type == tt_assign ||
                nextToken->token_type == tt_left_paren) {
                validExp = 0;
            }
            token_prev();
            break;
        case tt_lit_Bool:
        case tt_lit_Double:
        case tt_lit_Int:
        case tt_lit_String:
        case tt_kw_nil:
        case tt_plus:
        case tt_minus:
        case tt_times:
        case tt_divide:
        case tt_left_paren:
        case tt_right_paren:
        case tt_equals:
        case tt_not_equals:
        case tt_less_than:
        case tt_greater_than:
        case tt_less_equals:
        case tt_greater_equals:
        case tt_yolo:
        case tt_option:
            break;
        default:
            validExp = 0;
            break;
        }

        StackItem stackTop = stack_top(stack);
        // Binds token to a StackItem for precedence comparison
        StackItem newToken =
            (StackItem)sgc_malloc(sizeof(struct expressionStackItem));
        newToken->token = token;
        newToken->is_expression = 0;
        newToken->is_dollar = 0;
        // Reached the end of expression
        if (validExp == 0) {
            newToken->is_dollar = 1;
        }

        // If stackTop is expression, get symbol before expression
        if (stackTop->is_expression == 1) {
            StackItem temp = stack_pop(stack);
            stackTop = stack_top(stack);
            stack_push(stack, temp);
        }

        // If stackTop is not expression, get precedence
        PrecedenceTable precedence = get_precedence(stackTop, newToken);
        // If precedence is reduce, reduce expression
        while (precedence == R) {
            reduce(stack);
            stackTop = stack_top(stack);

            // If stackTop is expression, get symbol before expression
            if (stackTop->is_expression == 1) {
                StackItem temp = stack_pop(stack);
                stackTop = stack_top(stack);
                stack_push(stack, temp);
            }

            precedence = get_precedence(stackTop, newToken);
        }

        // If precedence is shift, push token to stack
        // If precedence is error, print error
        // If precedence is end of expression, check expression and return
        if (precedence == S) {
            stack_push(stack, newToken);
        } else if (precedence == E) {
            error_print_exit(token, error_parser, "Invalid expression.");
        } else {
            stackTop = stack_top(stack);
            if (stackTop->is_expression == 1) {
                // Valid
                stack_destroy(&stack);
                return stackTop->exprInfo;
            } else {
                // No expression was found
                error_print_exit(token, error_parser, "Invalid expression.");
            }
        }

        token_next();
        token = token_peek();
    } // while(validExp)
    error_print_exit(token, error_internal, "Entered unreachable code");
}

void reduce(Stack stack) {
    StackItem top0 = stack_pop(stack);
    // If top0 is identifier or literal, convert to expression and push to stack
    if (top0->is_expression != 1 && is_identifier_or_literal(top0->token)) {
        top0->exprInfo = sem_term(top0->token);
        top0->is_expression = 1;
        stack_push(stack, top0);
        return;
    }

    StackItem top1 = stack_pop(stack);
    // If top0 is tt_yolo, only pop one more stackItem
    if (top1->is_expression == 1 && top0->token->token_type == tt_yolo) {
        StackItem newStackItem =
            (StackItem)sgc_malloc(sizeof(struct expressionStackItem));
        newStackItem->is_expression = 1;
        newStackItem->exprInfo = sem_yolo(top0->token, top1->exprInfo);
        stack_push(stack, newStackItem);
        return;
    }

    StackItem top2 = stack_pop(stack);

    if (top0->is_expression == 1 && is_infix_operator(top1->token) &&
        top2->is_expression == 1) {
        StackItem newStackItem =
            (StackItem)sgc_malloc(sizeof(struct expressionStackItem));
        newStackItem->is_expression = 1;
        switch (top1->token->token_type) {
        case tt_plus:
            newStackItem->exprInfo =
                sem_plus(top1->token, top2->exprInfo, top0->exprInfo);
            break;
        case tt_minus:
            newStackItem->exprInfo =
                sem_minus(top1->token, top2->exprInfo, top0->exprInfo);
            break;
        case tt_times:
            newStackItem->exprInfo =
                sem_times(top1->token, top2->exprInfo, top0->exprInfo);
            break;
        case tt_divide:
            newStackItem->exprInfo =
                sem_divide(top1->token, top2->exprInfo, top0->exprInfo);
            break;
        case tt_equals:
            newStackItem->exprInfo =
                sem_equals(top1->token, top2->exprInfo, top0->exprInfo);
            break;
        case tt_not_equals:
            newStackItem->exprInfo =
                sem_not_equals(top1->token, top2->exprInfo, top0->exprInfo);
            break;
        case tt_less_than:
            newStackItem->exprInfo =
                sem_less_than(top1->token, top2->exprInfo, top0->exprInfo);
            break;
        case tt_greater_than:
            newStackItem->exprInfo =
                sem_greater_than(top1->token, top2->exprInfo, top0->exprInfo);
            break;
        case tt_less_equals:
            newStackItem->exprInfo =
                sem_less_equals(top1->token, top2->exprInfo, top0->exprInfo);
            break;
        case tt_greater_equals:
            newStackItem->exprInfo =
                sem_greater_equals(top1->token, top2->exprInfo, top0->exprInfo);
            break;
        case tt_option:
            newStackItem->exprInfo =
                sem_option(top1->token, top2->exprInfo, top0->exprInfo);
            break;
        default:
            // Impossible state
            break;
        }

        stack_push(stack, newStackItem);
        return;
    } else if (top0->token->token_type == tt_right_paren &&
               top1->is_expression == 1 &&
               top2->token->token_type == tt_left_paren) {
        stack_push(stack, top1);
        return;
    }

    error_print_exit(top0->token, error_parser, "Invalid expression.");
}

int is_identifier_or_literal(Token *token) {
    switch (token->token_type) {
    case tt_ident:
    case tt_lit_Bool:
    case tt_lit_Double:
    case tt_lit_Int:
    case tt_lit_String:
    case tt_kw_nil:
        return 1;
    default:
        return 0;
    }
}

int is_infix_operator(Token *token) {
    switch (token->token_type) {
    case tt_plus:
    case tt_minus:
    case tt_times:
    case tt_divide:
    case tt_equals:
    case tt_not_equals:
    case tt_less_than:
    case tt_greater_than:
    case tt_less_equals:
    case tt_greater_equals:
    case tt_option:
        return 1;
    default:
        return 0;
    }
}

void stack_init(Stack *stack, int size) {
    if (stack == NULL) {
        return;
    }
    *stack = (Stack)sgc_malloc(sizeof(struct expressionStack));
    (*stack)->items =
        (StackItem *)sgc_malloc(sizeof(struct expressionStackItem) * size);
    (*stack)->top = -1;
    (*stack)->size = size;
}

void stack_push(Stack stack, StackItem item) {
    if (stack->top == stack->size - 1) {
        stack->size *= 2;
        stack->items = (StackItem *)sgc_realloc(
            stack->items, sizeof(struct expressionStackItem) * stack->size);
    }
    stack->items[++stack->top] = item;
}

StackItem stack_top(Stack stack) {
    if (stack->top == -1) {
        return NULL;
    }
    return stack->items[stack->top];
}

StackItem stack_pop(Stack stack) {
    if (stack->top == -1) {
        return NULL;
    }
    return stack->items[stack->top--];
}

void stack_destroy(Stack *stack) {
    sgc_free((*stack)->items);
    (*stack)->items = NULL;
    (*stack)->top = -1;
    (*stack)->size = 0;
}
