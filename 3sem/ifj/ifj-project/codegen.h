/*
 * codegen.h
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include "global_include.h"
#include "symtable.h"

/// initializes output buffer
void codegen_init();

/// appends formatted string to output buffer
void codegen_append(char *format, ...);

/// same as codegen_append, but
/// automatically appends newline
void codegen_append_newl(char *format, ...);

/// appends defvar instruction into serparate buffer
void codegen_append_defvar(VarInfo *var_info);

/// appends variable name in IFJcode23 format, eg. LF@var_name$42
void codegen_append_var(VarInfo *var_info);

/// appends integer symbol, eg int@42
void codegen_append_int(int value);

/// appends double symbol, eg. float@0x1.2666666666666p+0
void codegen_append_double(double value);

/// appends string symbol, formatted according to assignment
void codegen_append_string(char *value);

/// appends bool symbol, eg bool@false
void codegen_append_bool(bool value);

/// appends nil symbol, nil@nil
void codegen_append_nil();

/// initializes buffers for definitions and code,
/// generates label and pushframe instructions
void codegen_enter_func(Token *func_name);

/// flushes definitions and code info another buffer
void codegen_leave_func();

/// prints buffer content to stdout
void codegen_print();

/// deallocates output buffer
void codegen_destroy();

#endif // CODEGEN_H
