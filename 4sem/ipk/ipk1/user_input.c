#include "user_input.h"
#include "imports.h"
#include "option.h"
#include "utils.h"
#include "validation.h"
#include "vec.h"

GEN_OPTION(UserInput, user_input)

#define parse_param(name, length, validation_function)                         \
    user_input.name = strtok(NULL, " ");                                       \
    if (user_input.name == NULL) {                                             \
        eprintf("Not enough arguments for %s", command);                       \
        goto ERR_INVALID_INPUT;                                                \
    }                                                                          \
    if (validate_content(raw_input,                                            \
                         (i32)(user_input.name - (char *)raw_input.content),   \
                         length, validation_function) == -1) {                 \
        eprintf("Invalid content for argument %s (invalid characters, too "    \
                "long, ...)",                                                  \
                #name);                                                        \
        goto ERR_INVALID_INPUT;                                                \
    }

#define assert_end()                                                           \
    {                                                                          \
        char *next_token = strtok(NULL, "");                                   \
        if (next_token != NULL) {                                              \
            eprintf("Too many arguments for %s", command);                     \
            goto ERR_INVALID_INPUT;                                            \
        }                                                                      \
    }

const char *chat_help_message =
    "Available commands:"
    "\n/auth {Username} {Secret} {DisplayName}"
    "\n    - Sends authentication data to the server and sets the display name."
    "\n/join {ChannelID}"
    "\n    - Joins the specified channel."
    "\n/rename {DisplayName}"
    "\n    - Changes your display name locally."
    "\n/help"
    "\n    - Prints this help.";

bool is_command_name_char(u8 c) { return is_alphanumdash(c) || c == '_'; }

bool starts_with(u8Vec data, u32 start, char *pattern) {
    u32 length = (u32)strlen(pattern);
    if (data.size - start < length) {
        return false;
    }

    for (u32 i = 0; i < length; i++) {
        if (data.content[start + i] != pattern[i]) {
            return false;
        }
    }

    return true;
}

UserInputOption parse_user_input(u8Vec raw_input) {
    { // avoid -Wjump-misses-init on gotos
        UserInput user_input = {.raw_input = raw_input,
                                .command_type = COMMAND_MSG,
                                .username = NULL,
                                .secret = NULL,
                                .display_name = NULL,
                                .channel_id = NULL,

                                .message = NULL};

        if (raw_input.size < 1) {
            goto ERR_INVALID_INPUT;
        }

        // terminate vec with null byte
        i32 end = u8_vec_find(raw_input, '\n', 0);
        if (end == -1 || !is_printable_string(raw_input)) {
            goto ERR_INVALID_INPUT;
        }
        raw_input.content[end] = '\0';

        // skip empty line
        if (raw_input.size == 1) {
            goto ERR_INVALID_INPUT;
        }

        // regular message, not a command
        if (raw_input.content[0] != '/' ||
            !is_command_name_char(raw_input.content[1])) {
            if (validate_content(raw_input, 0, 1400, is_printable) != -1) {
                user_input.command_type = COMMAND_MSG;
                user_input.message = (char *)raw_input.content;
                return user_input_option_some(user_input);
            } else {
                eprintf("Invalid message (illegal characters/too long)");
                goto ERR_INVALID_INPUT;
            }
        }

        char *command = strtok((char *)raw_input.content, " ");
        if (command == NULL) {
            goto ERR_INVALID_INPUT;
        }

        if (string_eq(command, "/auth")) {
            user_input.command_type = COMMAND_AUTH;
            parse_param(username, 20, is_alphanumdash);
            parse_param(secret, 128, is_alphanumdash);
            parse_param(display_name, 20, is_printable_nospace);
            assert_end();

        } else if (string_eq(command, "/join")) {
            user_input.command_type = COMMAND_JOIN;
            parse_param(channel_id, 20, is_alphanumdash);
            assert_end();

        } else if (string_eq(command, "/rename")) {
            user_input.command_type = COMMAND_RENAME;
            parse_param(display_name, 20, is_printable_nospace);
            assert_end();

        } else if (string_eq(command, "/help")) {
            puts(chat_help_message);
            goto ERR_INVALID_INPUT;

        } else {
            eprintf("Unknown command: %s", command);
            goto ERR_INVALID_INPUT;
        }

        return user_input_option_some(user_input);
    }

ERR_INVALID_INPUT:
    u8_vec_clear(&raw_input);
    return user_input_option_none();
}

void free_user_input(UserInput user_input) {
    u8_vec_clear(&user_input.raw_input);
}
