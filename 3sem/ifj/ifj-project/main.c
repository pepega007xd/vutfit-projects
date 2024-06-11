/*
 * main.c
 *
 * xhashm00 Abas Hashmy
 * xbrabl04 Tomáš Brablec
 *
 */

#include "codegen.h"
#include "rec_parser.h"
#include "sem_find_functions.h"
#include "token_table.h"

int main() {
    sgc_init();         // initialize memory allocator
    init_token_table(); // fill token table
    sem_init();         // initialize semantic analyzer and codegen
    find_functions();   // find all functions in file and parses their headers

    parse_file(); // run parser

    codegen_print(); // print generated code to stdout
    sgc_free_all();  // free all allocated memory throughout program
    sgc_destroy();   // destroy memory allocator

    return EXIT_SUCCESS;
}
