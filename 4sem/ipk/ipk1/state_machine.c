#include "config.h"
#include "imports.h"
#include "input.h"
#include "message.h"
#include "sending.h"
#include "state_machine_utils.h"
#include "user_input.h"
#include "utils.h"

extern Config config;

FSMTermination fsm_termination = {.kind = FSM_TERMINATION_NONE};

void run_state_machine(void) {
    Input input;
    State state = STATE_START;
    bool waiting_for_reply = false;

    while (true) {
        // if any of the operations inside the FSM switch requires a state
        // change, handle it before reading input
        handle_fsm_termination(state != STATE_START);

        // waits until we get an input
        input = get_input(!waiting_for_reply);

        if (input.type == SERVER_MESSAGE) {
            print_message(input.server_message);
            if (input.server_message.message_type == MESSAGE_ERR) {
                fsm_termination.kind = FSM_TERMINATION_BYE;
            }
        }

        // if any of the operations inside get_input requires a state change,
        // handle it before going into the FSM switch
        handle_fsm_termination(state != STATE_START);

        // implements state machine from the assignment, states ERROR and BYE
        // are handled in function `handle_fsm_termination`
        switch (state) {
        case STATE_START:
            if (IS_AUTH_COMMAND) {
                send_user_input(input.user_input);
                waiting_for_reply = true;
                state = STATE_AUTH;
            } else if (input.type == USER_INPUT) {
                illegal_user_input();
            } else {
                invalid_server_message();
            }
            break;

        case STATE_AUTH:
            if (IS_VALID_REPLY_MESSAGE) {
                waiting_for_reply = false;

                if (input.server_message.reply_result) {
                    state = STATE_OPEN;
                }
            } else if (IS_AUTH_COMMAND) {
                send_user_input(input.user_input);
                waiting_for_reply = true;
            } else if (input.type == USER_INPUT) {
                illegal_user_input();
            } else {
                invalid_server_message();
            }
            break;

        case STATE_OPEN:
            if (IS_VALID_REPLY_MESSAGE) {
                waiting_for_reply = false;
            } else if (IS_MSG_MESSAGE) {
                continue; // already printed
            } else if (IS_ERR_MESSAGE) {
                fsm_termination.kind = FSM_TERMINATION_BYE;
            } else if (IS_BYE_MESSAGE) {
                fsm_termination.kind = FSM_TERMINATION_SILENT;
            } else if (IS_MSG_COMMAND) {
                send_user_input(input.user_input);
            } else if (IS_JOIN_COMMAND) {
                waiting_for_reply = true;
                send_user_input(input.user_input);
            } else if (input.type == USER_INPUT) {
                illegal_user_input();
            } else {
                invalid_server_message();
            }
            break;
        }

// No action needed, input memory is managed elsewhere

    }
}
