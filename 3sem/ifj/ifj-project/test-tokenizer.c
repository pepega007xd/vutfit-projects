/*
 * test-tokenizer.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "buffer.h"
#include "global_include.h"
#include "token_table.h"
#include "tokenizer.h"
#include <assert.h>
#include <string.h>

extern Buffer input;
extern TokenTable token_table;

// replaces the source code buffer that is used by tokenizer
void inject_code(char *code) {
    input = buffer_create();
    buffer_append(&input, code);
    // print_tokens();
}

#define assert_token(type)                                                     \
    {                                                                          \
        Token token = get_token();                                             \
        assert(token.token_type == type);                                      \
        assert(!token.after_newline);                                          \
    }

#define assert_token_ident(expected)                                           \
    {                                                                          \
        Token token = get_token();                                             \
        assert(token.token_type == tt_ident);                                  \
        assert(!strcmp(expected, token.content.ident));                        \
    }

#define assert_token_int(expected)                                             \
    {                                                                          \
        Token token = get_token();                                             \
        assert(token.token_type == tt_lit_Int);                                \
        assert(expected == token.content.int_lit);                             \
    }

#define assert_token_double(expected)                                          \
    {                                                                          \
        Token token = get_token();                                             \
        assert(token.token_type == tt_lit_Double);                             \
        assert(!strcmp(expected, token.content.double_lit));                   \
    }

#define assert_token_string(expected)                                          \
    {                                                                          \
        Token token = get_token();                                             \
        assert(token.token_type == tt_lit_String);                             \
        assert(!strcmp(expected, token.content.string_lit));                   \
    }

int main(void) {
    sgc_init();

    // sanity check
    inject_code("let a = 5 + b");
    assert_token(tt_kw_let);
    assert_token_ident("a");
    assert_token(tt_assign);
    assert_token_int(5);
    assert_token(tt_plus);
    assert_token_ident("b");
    assert_token(tt_EOF);

    inject_code(" ( \n )  ! ! \n! ! \n");
    assert_token(tt_left_paren);
    assert_token(tt_newl);
    assert_token(tt_right_paren);
    assert_token(tt_yolo);
    assert_token(tt_yolo);
    assert_token(tt_newl);
    assert_token(tt_yolo);
    assert_token(tt_yolo);
    assert_token(tt_newl);
    assert_token(tt_EOF);

    return 0;
}
