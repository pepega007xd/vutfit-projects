#ifndef USER_INPUT_H
#define USER_INPUT_H

#include "imports.h"
#include "option.h"
#include "vec.h"

typedef enum {
    COMMAND_MSG,   // regular message
    COMMAND_AUTH,  // `auth` command
    COMMAND_JOIN,  // `join` command
    COMMAND_RENAME // filtered out in stdin_io.c, will not reach FSM
} CommandType;

/// represents any valid user input (either command, or regular message)
typedef struct {
        // all `char *` fields of this struct point to this buffer,
        // it needs to be deallocated when destroying the object
        u8Vec raw_input;
        CommandType command_type;
        char *username;
        char *secret;
        char *display_name;
        char *channel_id;

        char *message; // content of regular message
} UserInput;

GEN_OPTION_H(UserInput, user_input)

/// parses a single line of user input, returns UserInput object with the vector
/// inside when parsing succeeds, when parsing fails, the vector is deallocated
UserInputOption parse_user_input(u8Vec raw_input);

/// deallocates UserInput object
void free_user_input(UserInput user_input);

#endif // !USER_INPUT_H
