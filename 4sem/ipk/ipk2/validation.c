#include "imports.h"
#include "vec.h"

bool is_printable(u8 c) {
    u8 lower_bound = 0x20;
    u8 upper_bound = 0x7e;

    return lower_bound <= c && c <= upper_bound;
}

bool is_printable_nospace(u8 c) {
    u8 lower_bound = 0x21;
    u8 upper_bound = 0x7e;

    return lower_bound <= c && c <= upper_bound;
}

bool is_alphanumdash(u8 c) { return isalpha(c) || isdigit(c) || c == '-'; }

i32 validate_content(u8Vec raw_message, i32 start, i32 max_length,
                     bool (*is_valid_char)(u8 c)) {
    if (start < 0) {
        return -1;
    }

    i32 end = u8_vec_find(raw_message, '\0', start);
    i32 length = end - start;
    if (end == -1 || length <= 0 || length > max_length) {
        return -1;
    }

    for (i32 i = start; i < end; i++) {
        if (!is_valid_char(raw_message.content[i])) {
            return -1;
        }
    }

    return end;
}
