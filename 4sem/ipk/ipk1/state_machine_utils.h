#ifndef STATE_MACHINE_UTILS_H
#define STATE_MACHINE_UTILS_H

#include "imports.h"
#include "message.h"

typedef enum {
    FSM_TERMINATION_NONE,
    FSM_TERMINATION_SILENT,
    FSM_TERMINATION_BYE,
    FSM_TERMINATION_ERROR
} FSMTerminationKind;

/// used to signal that the event loop should terminate from many places
/// in the code, to avoid passing a pointer everywhere, it is a global variable
typedef struct {
        FSMTerminationKind kind;
        char *error_message; // valid only in state FSM_TERMINATION_ERROR
} FSMTermination;

typedef enum {
    STATE_START,
    STATE_AUTH,
    STATE_OPEN,
} State;

#define illegal_user_input()                                                   \
    eprintf("Cannot use this command / send messages at this state");

#define invalid_server_message()                                               \
    fsm_termination.kind = FSM_TERMINATION_ERROR;                              \
    fsm_termination.error_message = "Received invalid message from server";

/// returns name of state as a string
char *str_state(State state);

/// checks if message is a valid reply at this state (in UDP mode, it checks
/// that the message ID matches the original AUTH/JOIN message)
bool is_valid_reply(Message message);

/// possibly terminates the program according to the `fsm_termination` global
/// variable, optionally sends the ERR and BYE messages
void handle_fsm_termination(bool send_messages);

// these #defines are here just to make the FSM switch more readable

#define IS_AUTH_COMMAND                                                        \
    (input.type == USER_INPUT && input.user_input.command_type == COMMAND_AUTH)

#define IS_MSG_COMMAND                                                         \
    (input.type == USER_INPUT && input.user_input.command_type == COMMAND_MSG)

#define IS_VALID_REPLY_MESSAGE                                                 \
    (waiting_for_reply && input.type == SERVER_MESSAGE &&                      \
     input.server_message.message_type == MESSAGE_REPLY &&                     \
     is_valid_reply(input.server_message))

#define IS_MSG_MESSAGE                                                         \
    (input.type == SERVER_MESSAGE &&                                           \
     input.server_message.message_type == MESSAGE_MSG)

#define IS_ERR_MESSAGE                                                         \
    (input.type == SERVER_MESSAGE &&                                           \
     input.server_message.message_type == MESSAGE_ERR)

#define IS_BYE_MESSAGE                                                         \
    (input.type == SERVER_MESSAGE &&                                           \
     input.server_message.message_type == MESSAGE_BYE)

#define IS_JOIN_COMMAND                                                        \
    (input.type == USER_INPUT && input.user_input.command_type == COMMAND_JOIN)

#endif // !STATE_MACHINE_UTILS_H
