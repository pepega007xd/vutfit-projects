/*
 * codegen.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

#include "codegen.h"
#include "buffer.h"
#include "sem_state.h"
#include "sgc.h"
#include "symtable.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

Buffer global_code;
Buffer function_code;

Buffer global_defs;
Buffer local_defs;

// used for storing function code of function being generated, after generating
// the whole function, this buffer is flushed into function_code after flushing
// function_defs of the same function
Buffer local_code;

extern SemState sem_state;

// in file builtin_functions.c
extern char *builtin_functions;

void codegen_init() {
    global_code = buffer_create();
    function_code = buffer_create();
    global_defs = buffer_create();
}

// prints to code buffers
void append_variadic(Buffer *buffer, char *format, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);

    // if we're in a function, we must write the code into a separate buffer
    // that will be appended to output at the end - function bodies cannot
    // interleave with main program code

    int output_size = vsnprintf(NULL, 0, format, args);
    assert(output_size >= 0); // negative size means error
    char *tmp_output = sgc_malloc((unsigned)output_size + 1);
    vsprintf(tmp_output, format, args_copy);
    buffer_append(buffer, tmp_output);
    sgc_free(tmp_output);

    va_end(args_copy);
}
Buffer *get_code_buffer() {
    if (sem_in_local()) {
        return &local_code;
    } else {
        return &global_code;
    }
}

void codegen_append(char *format, ...) {
    va_list args;
    va_start(args, format);

    append_variadic(get_code_buffer(), format, args);

    va_end(args);
}

void codegen_append_newl(char *format, ...) {
    va_list args;
    va_start(args, format);

    Buffer *code_buffer = get_code_buffer();
    append_variadic(code_buffer, format, args);
    buffer_push(code_buffer, '\n');

    va_end(args);
}

void append_direct_function_code(char *format, ...) {
    va_list args;
    va_start(args, format);

    append_variadic(&function_code, format, args);

    va_end(args);
}

void append_def(char *format, ...) {
    va_list args;
    va_start(args, format);

    Buffer *def_buffer;
    if (sem_in_local()) {
        def_buffer = &local_defs;
    } else {
        def_buffer = &global_defs;
    }
    append_variadic(def_buffer, format, args);

    va_end(args);
}

void codegen_append_defvar(VarInfo *var_info) {
    append_def("DEFVAR ");
    append_def("%s@", var_info->is_local ? "LF" : "GF");
    append_def("%s$", var_info->declaration_token->content.ident);
    append_def("%u", var_info->id);
    append_def("\n");
}

// appends variable name in IFJcode23 format, eg. LF@var_name$42
void codegen_append_var(VarInfo *var_info) {
    codegen_append("%s@", var_info->is_local ? "LF" : "GF");
    codegen_append("%s$", var_info->declaration_token->content.ident);
    codegen_append("%u", var_info->id);
}

void codegen_append_int(int value) { codegen_append("int@%d", value); }

void codegen_append_double(double value) { codegen_append("float@%a", value); }

bool escaped(unsigned char c) {
    if (c < 33 || c == '#' || c == '\\' || c >= 127) {
        return true;
    } else {
        return false;
    }
}

void codegen_append_string(char *value) {
    codegen_append("string@");

    unsigned char c = (unsigned char)*value++;
    while (c != '\0') {
        if (escaped(c)) {
            codegen_append("\\%03u", c); // formatted as \xyz
        } else {
            codegen_append("%c", c);
        }
        c = (unsigned char)*value++;
    }
}

void codegen_append_bool(bool value) {
    codegen_append("bool@%s", value ? "true" : "false");
}

void codegen_append_nil() { codegen_append("nil@nil"); }

void codegen_enter_func(Token *func_name) {
    append_direct_function_code("LABEL %s\n", func_name->content.ident);
    append_direct_function_code("PUSHFRAME\n");
    local_defs = buffer_create();
    local_code = buffer_create();
}

void codegen_leave_func() {
    // definitions are flushed before code
    buffer_append(&function_code, local_defs.content);
    buffer_append(&function_code, local_code.content);
    buffer_destroy(&local_defs);
    buffer_destroy(&local_code);
}

void codegen_print() {
    puts(".IFJcode23");

    // used for temporary pop of values from stack during type casting
    puts("DEFVAR GF@$lhs");
    puts("DEFVAR GF@$rhs");
    puts("DEFVAR GF@$tmp");

    // definitions are flushed before code
    buffer_print(&global_defs, stdout);
    buffer_print(&global_code, stdout);

    puts("EXIT int@0"); // end of main body
    buffer_print(&function_code, stdout);
    puts(builtin_functions);
}

void codegen_destroy() {
    buffer_destroy(&function_code);
    buffer_destroy(&global_defs);
}
