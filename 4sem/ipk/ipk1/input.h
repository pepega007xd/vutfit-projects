#ifndef INPUT_H
#define INPUT_H

#include "imports.h"
#include "message.h"
#include "user_input.h"

typedef enum {
    SERVER_MESSAGE, // message received from server
    USER_INPUT      // command or message read from standard input
} InputType;

// Represents a new input to the program's event loop,
// either a message from the server, or from the user via standard input
typedef struct {
        InputType type;
        Message server_message;
        UserInput user_input;
} Input;

// returns the next available user input, waits using `poll`
//
// in UDP mode, retransmissions are handled using timeouts
//
// in case of error, the `fsm_termination` variable is set
Input get_input(bool read_from_stdin);

#endif // !INPUT_H
