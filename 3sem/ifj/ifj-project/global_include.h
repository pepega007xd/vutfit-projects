/*
 * global_include.h
 *
 * xhashm00 Abas Hashmy
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef GLOBAL_INCLUDE_H
#define GLOBAL_INCLUDE_H

#include "sgc.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {

    // newline
    tt_newl, // \n (cf, lf)

    tt_EOF, // EOF

    // keywords
    tt_kw_Double,
    tt_kw_Double_opt, // Double?
    tt_kw_else,
    tt_kw_func,
    tt_kw_if,
    tt_kw_Int,
    tt_kw_Int_opt, // Int?
    tt_kw_let,          //let je konstanta
    tt_kw_nil,      //null
    tt_kw_return,
    tt_kw_String,
    tt_kw_String_opt, // String?
    tt_kw_var,        //tokem key word var - var je proměnná
    tt_kw_while,
    tt_kw_Bool,
    tt_kw_Bool_opt, // Bool?
    tt_kw_true,
    tt_kw_false,

    // Indentificator - když máme int i = 7 -- identifikátor je i
    tt_ident,

    // literals - např 42, řetězec "Hallo world"; true -- literaly values
    tt_lit_Int,
    tt_lit_Double,
    tt_lit_String,
    tt_lit_Bool,

    // symbols
    tt_left_paren,  // (
    tt_right_paren, // )
    tt_colon,       // :
    tt_arrow,       // ->
    tt_left_curly,  // {
    tt_right_curly, // }
    tt_assign,      // =
    tt_underscore,  // _
    tt_comma,       // ,

    // operators
    tt_plus,   // +
    tt_minus,  // -
    tt_times,  // *
    tt_divide, // /
    tt_option, // ??
    tt_yolo,   // !

    // relation operators
    tt_equals,        // ==
    tt_not_equals,    // !=
    tt_less_than,     // <
    tt_greater_than,  // >
    tt_less_equals,   // <=
    tt_greater_equals // >=

} TokenType;

// union for potential token contents
typedef union {
        int int_lit;
        double double_lit;
        char *string_lit;
        bool bool_lit;
        char *ident;
} TokenContent;

// token
typedef struct {
        TokenType token_type;
        TokenContent content;
        unsigned line_number;  // Line number for printed message
        unsigned char_column;  // Column of the token
        char *line_start;      // Pointer to the line of token
        unsigned token_length; // Length of the token for printed message
        bool after_newline;    // true if token was preceeded by newline
} Token;

// error codes
typedef enum error_type {
    // chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního
    // lexému
    error_lexer = 1,
    // chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu,
    // chybějící hlavička, atp.)
    error_parser = 2,
    // sémantická chyba v programu – nedefinovaná funkce, redefinice proměnné
    error_ident = 3,
    // sémantická chyba v programu – špatný počet/typ parametrů u volání funkce
    // či špatný typ návratové hodnoty z funkce
    error_call = 4,
    // sémantická chyba v programu – použití nedefinované nebo neinicializované
    // proměnné
    error_undefined_var = 5,
    // sémantická chyba v programu – chybějící/přebývající výraz v příkazu
    // návratu z funkce
    error_invalid_return = 6,
    // sémantická chyba typové kompatibility v aritmetických, řetězcových a
    // relačních výrazech
    error_type = 7,
    // sémantická chyba odvození typu – typ proměnné nebo parametru není uveden
    // a nelze odvodit od použitého výrazu
    error_type_inference = 8,
    // ostatní sémantické chyby
    error_other_semantic = 9,
    // interní chyba překladače tj. neovlivněná vstupním programem (např. chyba
    // alokace paměti atd.)
    error_internal = 99
} ErrorType;

// error function
_Noreturn void error_print_exit(Token *token, ErrorType type, char *err_msg);

#endif // GLOBAL_INCLUDE_H
