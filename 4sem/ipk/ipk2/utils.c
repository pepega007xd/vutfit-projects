#include "connection.h"
#include "imports.h"
#include "user.h"

extern Connection connection;
extern UserVec users;

void eprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "ERR: ");
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
    va_end(args);
}

void exit_fail(void) {
    // close all file descriptors
    close(connection.epoll_fd);
    close(connection.tcp_auth_socket);
    close(connection.udp_auth_socket);

    for (u64 i = 0; i < users.size; i++) {
        close(users.content[i].user_socket);
    }

    // free all memory
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

bool string_is_empty(const char *string) { return strlen(string) == 0; }

i64 get_time_ms(void) {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000 + now.tv_usec / 1000;
}
