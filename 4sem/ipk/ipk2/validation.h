#ifndef VALIDATION_H
#define VALIDATION_H

#include "imports.h"
#include "vec.h"

/// returns true if character is printable (0x20 - 0x7f inclusive)
bool is_printable(u8 c);

/// returns true if character is printable, but not a space (0x21 - 0x7f
/// inclusive)
bool is_printable_nospace(u8 c);

/// returns true if character is either ascii alphanumeric, or a dash (-)
bool is_alphanumdash(u8 c);

/// Checks that string starting at `start` has maximum length of `max_length`,
/// and its chars are valid (checked using `is_valid_char`).
///
/// Returns the index of null terminating byte, or -1 on error.
i32 validate_content(u8Vec raw_message, i32 start, i32 max_length,
                     bool (*is_valid_char)(u8 c));

#endif // !VALIDATION_H
