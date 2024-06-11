#include "connection.h"
#include "imports.h"

void exit_fail(void) {
    close_connection();
    sgc_free_all();
    exit(EXIT_FAILURE);
}

void print_exit(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "ERR: ");
    vfprintf(stderr, format, args);
    fputc('\n', stderr);

    va_end(args);
    exit_fail();
}

bool string_eq(const char *lhs, const char *rhs) {
    return strcmp(lhs, rhs) == 0;
}

bool string_eq_nocase(const char *lhs, const char *rhs) {
    u64 lhs_len = strlen(lhs);
    u64 rhs_len = strlen(rhs);
    if (lhs_len != rhs_len) {
        return false;
    }

    for (u64 i = 0; i < lhs_len; i++) {
        if (toupper(lhs[i]) != toupper(rhs[i])) {
            return false;
        }
    }

    return true;
}

u16 from_net_bytes(u8 byte0, u8 byte1) {
    u16 value = (u16)(byte1 << 8) | byte0;
    return ntohs(value);
}

void eprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "ERR: ");
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
    va_end(args);
}
