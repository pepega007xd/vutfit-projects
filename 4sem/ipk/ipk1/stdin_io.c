#include "imports.h"
#include "state_machine_utils.h"
#include "user_input.h"
#include "utils.h"
#include "vec.h"

/// stores the last value of display name, only modified here
/// by receiving the auth or rename command
char display_name[21];

extern FSMTermination fsm_termination;

UserInputOption receive_user_input(void) {
    static u8Vec input_buffer;

    // read bytes from socket up to '\n' - single message
    while (true) {
        u8 input_byte;
        i64 size = read(STDIN_FILENO, &input_byte, 1);

        if (size == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
            return user_input_option_none();
        }

        else if (size == -1 && errno == EINTR) {
            continue;

        } else if (size == -1) {
            print_exit("Failed to read from stdin: %s", strerror(errno));

        } else if (size == 0) {
            // end of input
            fsm_termination.kind = FSM_TERMINATION_BYE;
            return user_input_option_none();

        } else {
            u8_vec_push(&input_buffer, input_byte);

            if (input_byte == '\n') {
                break;
            }
        }
    }

    UserInputOption user_input = parse_user_input(input_buffer);

    input_buffer = (u8Vec){0};

    if (!user_input.is_valid) {
        return user_input;
    }

    // extract display name from AUTH and RENAME commands
    if (user_input.content.command_type == COMMAND_AUTH ||
        user_input.content.command_type == COMMAND_RENAME) {
        memset(display_name, '\0', 21);
        strcpy(display_name, user_input.content.display_name);
    }

    if (user_input.content.command_type == COMMAND_RENAME) {
        free_user_input(user_input.content);
        return user_input_option_none();
    }

    return user_input;
}
