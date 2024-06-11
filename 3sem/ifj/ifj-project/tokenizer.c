/*
 * tokenizer.c
 *
 * xhashm00 Abas Hashmy
 * xbrabl04 Tomáš Brablec
 *
 */

#include "tokenizer.h"
#include "buffer.h"
#include "global_include.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// load input code to buffer
Buffer input;

void tokenizer_init() {
    input = buffer_create();
    /// Loop to fill input buffer.
    int c;
    while ((c = getchar()) != EOF) {
        buffer_push(&input, (char)c);
    }
}

#define tokenizer_error()                                                      \
    {                                                                          \
        fprintf(stderr, "Tokenizer error on line %d.\n", __LINE__);            \
        exit(error_lexer);                                                     \
    }

#define matches_with_opt(expected, tt_non_opt, tt_opt)                         \
    if (!strcmp(token_content, expected)) {                                    \
        if (peek_next_char(&input) == '?') {                                   \
            consume_next_char(&input);                                         \
            return tt_opt;                                                     \
        }                                                                      \
        return tt_non_opt;                                                     \
    }

#define matches_with(expected, tt)                                             \
    if (!strcmp(token_content, expected)) {                                    \
        return tt;                                                             \
    }

/// checks whether scanned identifier is one of known keywords,
/// returns appropriate token_type when keyword is found,
/// and tt_ident otherwise
TokenType determine_ident_or_kw(char *token_content) {
    matches_with_opt("Double", tt_kw_Double, tt_kw_Double_opt);
    matches_with_opt("Int", tt_kw_Int, tt_kw_Int_opt);
    matches_with_opt("String", tt_kw_String, tt_kw_String_opt);
    matches_with_opt("Bool", tt_kw_Bool, tt_kw_Bool_opt);

    matches_with("let", tt_kw_let);
    matches_with("var", tt_kw_var);

    matches_with("if", tt_kw_if);
    matches_with("else", tt_kw_else);

    matches_with("func", tt_kw_func);
    matches_with("nil", tt_kw_nil);
    matches_with("return", tt_kw_return);
    matches_with("while", tt_kw_while);
    matches_with("true", tt_kw_true);
    matches_with("false", tt_kw_false);

    return tt_ident;
}

void token_finalize(Buffer *lexeme, Token *token, TokenType type) {

    token->token_type = type;
    token->line_number = input.line;
    token->char_column = input.column - lexeme->length;
    token->token_length = (unsigned)lexeme->length;
    token->line_start = &input.content[input.index] - input.column;
    token->after_newline = false;
}

/// Adds value to one buffer and consumes a character without storing it
/// from another one.
void push_and_consume(Buffer *lexeme, Buffer *consume, char c) {
    buffer_push(lexeme, c);
    consume_next_char(consume);
}

/// Function that completes a whole numeric literal and returns
/// true. If it returns false, the lexeme is not a numeric literal.
/// lexeme is and empty buffer and token is an empty token.
bool is_literal_number(Buffer *lexeme, Token *token) {
    char c = peek_char(&input);

    if (!isdigit(c)) {
        return false;
    }

    buffer_push(lexeme, c);

    typedef enum {
        F_Ilit,
        s_Dlitd,
        F_Dlitd,
        s_DlitE,
        s_DlitEo,
        F_DlitE
    } lit_num;

    lit_num state = F_Ilit;

    while (true) {
        c = peek_next_char(&input);
        switch (state) {

        case F_Ilit: /// F_ilit

            if (isdigit(c)) {
                state = F_Ilit;
            } else if (c == 'e' || c == 'E') {
                state = s_DlitE;
            } else if (c == '.') {
                state = s_Dlitd;
            } else {
                consume_next_char(&input);

                /// The next symbol doesn't belong to the lexeme.
                // push_and_consume(lexeme, &input, c);

                char *expected = lexeme->content + lexeme->length;
                char *actual;

                token->content.int_lit =
                    (int)strtol(lexeme->content, &actual, 10);

                if (expected != actual) {
                    tokenizer_error();
                }

                token_finalize(lexeme, token, tt_lit_Int);
                return true;
            }
            break;

        case s_DlitE: /// s_Dlit1
            if (isdigit(c)) {
                state = F_DlitE;
            } else if (c == '+' || c == '-') {
                state = s_DlitEo;
            } else {
                tokenizer_error();
            }
            break;

        case s_Dlitd: /// s_Dlit2
            if (isdigit(c)) {
                state = F_Dlitd;
            } else {
                tokenizer_error();
            }
            break;

        case s_DlitEo: /// s_Dlit3
            if (isdigit(c)) {
                state = F_DlitE;
            } else {
                tokenizer_error();
            }
            break;

        case F_Dlitd: /// F_Dlit1
            if (isdigit(c)) {
                state = F_Dlitd;
            } else if (c == 'e' || c == 'E') {
                state = s_DlitE;
            } else {
                /// The next symbol doesn't belong to the lexeme.
                consume_next_char(&input);

                /// Converting the string with numeric literal to number.
                char *expected = lexeme->content + lexeme->length;
                char *actual;

                token->content.double_lit = strtod(lexeme->content, &actual);

                if (expected != actual) {
                    tokenizer_error();
                }

                token_finalize(lexeme, token, tt_lit_Double);
                return true;
            }
            break;

        case F_DlitE: /// F_Dlit2
            if (isdigit(c)) {
                state = F_DlitE;
            } else {
                /// The next symbol doesn't belong to the lexeme.
                consume_next_char(&input);

                char *expected = lexeme->content + lexeme->length;
                char *actual;

                token->content.double_lit = strtod(lexeme->content, &actual);

                if (expected != actual) {
                    tokenizer_error();
                }

                token_finalize(lexeme, token, tt_lit_Double);
                return true;
            }
            break;
        }

        push_and_consume(lexeme, &input, c);
    }
    return false;
}

bool is_printable(char c) { return c > 31 && c < 127; }

/// Function that completes a whole single line string literal
/// and returns true.
/// If it returns false, the lexeme is not a string literal.
////// lexeme is and empty buffer and token is an empty token.
bool is_literal_string(Buffer *lexeme, Token *token) {
    typedef enum { s_Slit, F_Slit, s_Esc, s_EscU, s_EscU2, s_EscU3 } lit_str;

    lit_str state = s_Slit;
    char c;
    unsigned hex_code_idx = 0;
    char hex_code_buf[9] = {0};

    while (true) {
        c = peek_char(&input);

        switch (state) {

        case s_Slit:
            if (c == '"') {
                state = F_Slit;
            } else if (c == '\\') {
                state = s_Esc;
            } else if (is_printable(c)) {
                /// Above 31 are valid symbol value from assignment;
                buffer_push(lexeme, c);
                state = s_Slit;
            } else {
                tokenizer_error();
            }
            break;

        case s_Esc:
            if (c == '"' || c == '\\') {
                buffer_push(lexeme, c);
                state = s_Slit;
            } else if (c == 'n') {
                buffer_push(lexeme, '\n');
                state = s_Slit;
            } else if (c == 'r') {
                buffer_push(lexeme, '\r');
                state = s_Slit;
            } else if (c == 't') {
                buffer_push(lexeme, '\t');
                state = s_Slit;
            } else if (c == 'u') {
                state = s_EscU;
            } else {
                tokenizer_error();
            }
            break;

        case s_EscU:
            if (c == '{') {
                state = s_EscU2;
            } else {
                tokenizer_error();
            }
            break;

        case s_EscU2:
            if (isxdigit(c)) {
                hex_code_buf[hex_code_idx++] = c;
                state = s_EscU3;
            } else {
                tokenizer_error();
            }
            break;

        case s_EscU3:
            if (c == '}') {
                // we need to support just 0-255
                buffer_push(lexeme, (char)strtol(hex_code_buf, NULL, 16));
                memset(hex_code_buf, 0, 9);
                hex_code_idx = 0;

                state = s_Slit;
            } else if (isxdigit(c)) {
                if (hex_code_idx == 8) {
                    tokenizer_error();
                }
                hex_code_buf[hex_code_idx++] = c;

                state = s_EscU3;
            } else {
                tokenizer_error();
            }
            break;

        case F_Slit:
            token->content.string_lit = lexeme->content;
            token_finalize(lexeme, token, tt_lit_String);
            return true;
        }
        consume_next_char(&input);
    }
}

int max(int a, int b) { return a > b ? a : b; }

/// Loads the whole multiline string into a buffer tmp
/// to be processed further. Returns the number of spaces
/// between the ending trio of " and a previous newline.
unsigned load_string(Buffer *tmp) {
    char c = peek_char(&input);

    if (c != '\n') { /// Ignoring '\r'
        tokenizer_error();
    }

    int count = 0;
    while (count < 3) {

        if (c == 0) {
            tokenizer_error();
        }

        c = peek_next_char(&input);

        if (c == '"') {
            count++;
        } else {
            count = 0;
        }

        push_and_consume(tmp, &input, c);
    }
    consume_next_char(&input);

    unsigned limit = 0;

    for (int i = (int)tmp->length - 1; i >= 0; i--) {
        c = tmp->content[i];
        if (c == ' ') {
            limit++;
        } else if (c == '"') { // skip the double quotes
        } else if (c == '\n') {
            break;
        } else { // no other chars can be on the same line
            tokenizer_error();
        }
    }

    /// Ending the valid string.
    tmp->content[max((int)(tmp->length - 4 - limit), 0)] = 0;
    return limit;
}

// discards `limit` characters from `tmp`, errors if any one
// of them is not a space
void offset_string(Buffer *tmp, unsigned limit) {

    char c = 0;

    for (unsigned i = 0; i < limit; i++) {
        c = peek_char(tmp);

        if (i == 0 && (c == '\n' || c == '\0')) {
            // this is weird, but swift allows an empty line to not be indented,
            // but only if there really are *no* spaces (one to three, if limit
            // is 4, for example, is an error)
            return;
        }

        if (c != ' ') {
            tokenizer_error();
        }

        consume_next_char(tmp);
    }
}

void transfer_string(Buffer *lexeme, Buffer *tmp, unsigned limit) {

    typedef enum { s_Slit, s_Esc, s_EscU, s_EscU2, s_EscU3 } lit_str_l;

    lit_str_l state = s_Slit;

    offset_string(tmp, limit);

    char c = 0;
    char hex_code_buf[9] = {0};
    unsigned hex_code_idx = 0;

    while (true) {
        c = peek_char(tmp);
        switch (state) {

        case s_Slit:
            if (is_printable(c) && c != '\\') {

                buffer_push(lexeme, c);
                state = s_Slit;

            } else if (c == '\n') {
                buffer_push(lexeme, c);
                consume_next_char(tmp);
                offset_string(tmp, limit);
                continue;

            } else if (c == '\\') {

                state = s_Esc;

                /// EOF was not found while loading the string so 0 byte
                /// is used to end it.
            } else if (c == 0) {

                return;
            }
            break;

        case s_Esc:
            if (c == '\\' || c == '"') {
                buffer_push(lexeme, c);
                state = s_Slit;
            } else if (c == 'n') {
                buffer_push(lexeme, '\n');
                state = s_Slit;

            } else if (c == 'r') {
                buffer_push(lexeme, '\r');
                state = s_Slit;

            } else if (c == 't') {
                buffer_push(lexeme, '\t');
                state = s_Slit;

            } else if (c == 'u') {
                state = s_EscU;
            } else {
                tokenizer_error();
            }
            break;

        case s_EscU:
            if (c == '{') {
                state = s_EscU2;
            } else {
                tokenizer_error();
            }
            break;

        case s_EscU2:
            if (isalnum(c)) {
                hex_code_buf[hex_code_idx++] = c;
                state = s_EscU3;
            } else {
                tokenizer_error();
            }
            break;

        case s_EscU3:
            if (isalnum(c)) {
                if (hex_code_idx == 8) {
                    tokenizer_error();
                }
                hex_code_buf[hex_code_idx++] = c;
                state = s_EscU3;
            } else if (c == '}') {
                // we need to support just 0-255
                buffer_push(lexeme, (char)strtol(hex_code_buf, NULL, 16));
                hex_code_idx = 0;
                state = s_Slit;
            } else {
                tokenizer_error();
            }
            break;
        }

        consume_next_char(tmp);
    }
}

/// Function that completes a whole multiline string literal
/// and returns true.
/// If the function returns false, the lexeme is not a string literal.

bool is_literal_string_long(Buffer *lexeme, Token *token) {

    Buffer tmp = buffer_create();

    unsigned limit = load_string(&tmp);

    transfer_string(lexeme, &tmp, limit);

    token->content.string_lit = lexeme->content;
    token_finalize(lexeme, token, tt_lit_String);

    buffer_destroy(&tmp);
    return true;
}

/// Checks if the current lexeme can be a literal,
/// if yes returns true, if no returns false.
/// Lexem is passed to the function trough buffer.
bool is_literal(Buffer *lexeme, Token *token) {

    if (is_literal_number(lexeme, token)) {

        return true;
    }

    /// In order to split the string state machine into
    /// multiple functions we first need to decide if the
    /// lexeme is a string literal
    /// and what type (single or multi line).
    char c = peek_char(&input);

    /// Lexeme is string literal.
    if (c == '"') {

        c = consume_next_char(&input);

        if (c == '"' && peek_next_char(&input) == '"') {
            /// Lexeme is multi line string literal.
            consume_next_char(&input);
            consume_next_char(&input);
            if (is_literal_string_long(lexeme, token)) {
                return true;
            }

        } else {
            /// Lexeme is a single line string literal.
            if (is_literal_string(lexeme, token)) {
                token->content.string_lit = lexeme->content;
                return true;
            }

            /// Skipt the next two instances of " symbol.
            consume_next_char(&input);
            consume_next_char(&input);
        }
    }

    return false;
}

/// Checks if the lexeme is an operator.
/// (also evaluates includes relation operator !=)
bool is_stand_operator(Buffer *lexeme, Token *token) {

    char c = peek_char(&input);

    switch (c) {
    case '*':
        consume_next_char(&input);
        lexeme->length = 1;
        token_finalize(lexeme, token, tt_times);
        return true;

    case '+':
        consume_next_char(&input);
        lexeme->length = 1;
        token_finalize(lexeme, token, tt_plus);
        return true;

    case '!':
        consume_next_char(&input);
        if (peek_char(&input) != '=') {
            lexeme->length = 1;
            token_finalize(lexeme, token, tt_yolo);
        } else {
            consume_next_char(&input);
            lexeme->length = 2;
            token_finalize(lexeme, token, tt_not_equals);
        }
        return true;

    case '?':
        consume_next_char(&input);
        if (peek_char(&input) == '?') {
            consume_next_char(&input);
            lexeme->length = 2;
            token_finalize(lexeme, token, tt_option);
            return true;
        } else {
            tokenizer_error();
        }
    }

    return false;
}

/// Checks if lexeme is relation operator.
/// (also evaluates assignment operator =)
bool is_rel_operator(Buffer *lexeme, Token *token) {
    char c = peek_char(&input);

    switch (c) {
    case '=':
        consume_next_char(&input);
        if (peek_char(&input) != '=') {
            lexeme->length = 1;
            token_finalize(lexeme, token, tt_assign);
            return true;
        } else {
            consume_next_char(&input);
            lexeme->length = 2;
            token_finalize(lexeme, token, tt_equals);
            return true;
        }
        break;

    case '<':
        consume_next_char(&input);
        if (peek_char(&input) != '=') {
            lexeme->length = 1;
            token_finalize(lexeme, token, tt_less_than);
            return true;
        } else {
            consume_next_char(&input);
            lexeme->length = 2;
            token_finalize(lexeme, token, tt_less_equals);
            return true;
        }
        break;

    case '>':
        consume_next_char(&input);
        if (peek_char(&input) != '=') {
            lexeme->length = 1;
            token_finalize(lexeme, token, tt_greater_than);
            return true;
        } else {
            consume_next_char(&input);
            lexeme->length = 2;
            token_finalize(lexeme, token, tt_greater_equals);
            return true;
        }
        break;
    }

    return false;
}

/// Calls functions to check if the next lexeme is
/// an operator. Returns true if yes and false if not.
/// lexeme is an empty initialized buffer.
bool is_operator(Buffer *lexeme, Token *token) {

    if (is_stand_operator(lexeme, token)) {
        return true;
    } else if (is_rel_operator(lexeme, token)) {
        return true;
    }
    return false;
}

/// returns true, if `c` can be a part of identifier (alphanumeric, or _)
bool is_ident_char(char c) { return isalnum(c) || c == '_'; }

/// Returns true if the lexeme is a symbol.
bool is_symbol(Buffer *lexeme, Token *token) {

    char c = peek_char(&input);

    switch (c) {
    case '_':
        // identifier can start with _
        if (is_ident_char(peek_next_char(&input))) {
            return false;
        }
        consume_next_char(&input);
        lexeme->length = 1;
        token_finalize(lexeme, token, tt_underscore);
        return true;

    case '(':
        consume_next_char(&input);
        lexeme->length = 1;
        token_finalize(lexeme, token, tt_left_paren);
        return true;

    case ')':
        consume_next_char(&input);
        lexeme->length = 1;
        token_finalize(lexeme, token, tt_right_paren);
        return true;

    case ':':
        consume_next_char(&input);
        lexeme->length = 1;
        token_finalize(lexeme, token, tt_colon);
        return true;

    case '{':
        consume_next_char(&input);
        lexeme->length = 1;
        token_finalize(lexeme, token, tt_left_curly);
        return true;

    case '}':
        consume_next_char(&input);
        lexeme->length = 1;
        token_finalize(lexeme, token, tt_right_curly);
        return true;

    case ',':
        consume_next_char(&input);
        lexeme->length = 1;
        token_finalize(lexeme, token, tt_comma);
        return true;

    case '-':
        consume_next_char(&input);
        if (peek_char(&input) == '>') {
            /// This imposter here has nowhere else to go.
            lexeme->length = 2;
            consume_next_char(&input);
            token_finalize(lexeme, token, tt_arrow);
            return true;
        } else {

            lexeme->length = 1;
            token_finalize(lexeme, token, tt_minus);
            return true;
        }
    }

    return false;
}

/// Checks if the next lexeme is a comment and if so returns false.
/// If it is a division the function evaluates the token and returns true.
bool is_division(Buffer *lexeme, Token *token) {
    char c = peek_char(&input);

    if (c != '/') {
        return false;
    }

    /// Additional block comments counter.
    unsigned block_depth = 0;

    typedef enum { F_div, F_lcom, s_bcom1, s_bcom2, s_bcom3, s_bcom4 } comments;

    comments state = F_div;

    /// Loop that endlessly looks at the next character and
    /// consumes it at the end. Ending the look is a responsibility
    /// of the state machine whithin it.
    while (true) {
        c = consume_next_char(&input);
        switch (state) {

        // Lexeme division.
        case F_div:
            if (c == '/') {
                // Not division but a line comment.
                state = F_lcom;

            } else if (c == '*') {
                // Not a division but a block comment.
                state = s_bcom1;

            } else {
                // Finalizing division token;
                lexeme->length = 1;
                token_finalize(lexeme, token, tt_divide);
                return true;
            }
            break;

            // Line comment
        case F_lcom:
            // Newl symbol ends a line comment.
            if (c == '\n') {
                // consume_next_char(&input);
                return false;

                // Continue to read trough the comment.
            } else {
                state = F_lcom;
            }
            break;

            // Block comment.
        case s_bcom1:
            // Symbol / can mean nested block comment.
            if (c == '/') {
                state = s_bcom2;

                // Symbol * can mean end of a block comment.
            } else if (c == '*') {
                state = s_bcom3;

                // Continue to read trough the comment.
            } else if (c == '\0') {
                // code ends inside block comment => error
                tokenizer_error();
            } else {
                state = s_bcom1;
            }
            break;

            // Check for nrsted block comment.
        case s_bcom2:
            // If there is a nested block comment increase their counter.
            if (c == '*') {
                block_depth++;

                // Symbol / can mean nested block comment.
            } else if (c == '/') {
                state = s_bcom2;
            }

            state = s_bcom1;
            break;

            // Potential end of block comment.
        case s_bcom3:

            // End of block comment, go to check if it was a
            // nested one.
            if (c == '/') {
                state = s_bcom4;

                // Block comment continues.
            } else {
                state = s_bcom1;
            }
            break;

        case s_bcom4:
            // Was a nested comment.
            if (block_depth != 0) {
                block_depth--;
                state = s_bcom1;
                continue;

            } else {
                // End of whole block comment.
                return false;
            }
            break;
        }
    }

    return false;
}

/// returns true, when token type is one of four type specifiers
bool is_type(TokenType type) {
    if (type == tt_kw_Int || type == tt_kw_Double || type == tt_kw_String ||
        type == tt_kw_Bool) {
        return true;
    }
    return false;
}

TokenType convert_type_to_opt(TokenType type) {
    if (type == tt_kw_Int) {
        return tt_kw_Int_opt;
    }
    if (type == tt_kw_Double) {
        return tt_kw_Double_opt;
    }
    if (type == tt_kw_String) {
        return tt_kw_String_opt;
    }
    if (type == tt_kw_Bool) {
        return tt_kw_Bool_opt;
    }
    tokenizer_error();
}

/// Returns true if the lexeme is an indetificator or a keyword.
/// Returns false otherwise.
bool is_identif_or_kw(Buffer *lexeme, Token *token) {

    char c = peek_char(&input);

    if (!is_ident_char(c)) {
        return false;
    }

    // consume all identifier chars into `lexeme` buffer
    while (is_ident_char(c)) {
        push_and_consume(lexeme, &input, c);
        c = peek_char(&input);
    }

    token->content.ident = lexeme->content;
    TokenType token_type = determine_ident_or_kw(lexeme->content);

    // check for optional type specifier, eg. 'String?'
    if (is_type(token_type) && c == '?') {
        consume_next_char(&input);
        token_type = convert_type_to_opt(token_type);
    }
    token_finalize(lexeme, token, token_type);

    if (token->token_type != tt_ident) {
        buffer_destroy(lexeme);
        token->content.ident = NULL;
    }

    if (token->token_type == tt_kw_true) {
        token->token_type = tt_lit_Bool;
        token->content.bool_lit = true;
    }

    if (token->token_type == tt_kw_false) {
        token->token_type = tt_lit_Bool;
        token->content.bool_lit = false;
    }

    return true;
}

/// Returns true if the lexeme is a newline.
/// Returns false otherwise.
bool is_newl(Buffer *lexeme, Token *token) {

    char c = peek_char(&input);

    if (c == '\n') {
        consume_next_char(&input);
        lexeme->length = 1;
        token_finalize(lexeme, token, tt_newl);
        return true;
    }

    return false;
}

/// Returns true if lexeme is an EOF.
/// Returns false otherwise.
bool is_EOF(Buffer *lexeme, Token *token) {
    char c = peek_char(&input);

    if (c == 0) {
        lexeme->length = 0;
        token_finalize(lexeme, token, tt_EOF);
        return true;
    }

    return false;
}

/// Retruns true if current character in input buffer
/// is a whitespace or if the white space is a valid
/// token (\n, EOF).
/// Returns false otherwise.
bool is_whitespace() {

    char c = peek_char(&input);
    if (c == ' ' || c == '\t' || c == '\r') {
        consume_next_char(&input);
        return true;
    }

    return false;
}

/// Main tokenizer function,
/// builds and returns a token.
Token get_token() {

    Token token;
    Buffer lexeme;
    lexeme = buffer_create();

    /// Loop that eats all whitespaces except newline
    while (is_whitespace()) {
    }

    if (is_EOF(&lexeme, &token)) {
        buffer_destroy(&lexeme);

    } else if (is_division(&lexeme, &token)) {
        buffer_destroy(&lexeme);
    } else if (is_whitespace()) {
        // we needed to eat whitespace that might've been left out after comment
    } else if (is_newl(&lexeme, &token)) {
        buffer_destroy(&lexeme);

    } else if (is_symbol(&lexeme, &token)) {
        buffer_destroy(&lexeme);

    } else if (is_literal(&lexeme, &token)) {

    } else if (is_operator(&lexeme, &token)) {
        buffer_destroy(&lexeme);

    } else if (is_identif_or_kw(&lexeme, &token)) {
    } else {
        tokenizer_error();
    }

    return token;
}
