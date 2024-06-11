#include "state_machine_utils.h"
#include "config.h"
#include "connection.h"
#include "imports.h"
#include "message.h"
#include "sending.h"
#include "udp_io.h"
#include "utils.h"

extern Config config;
extern FSMTermination fsm_termination;
extern u16 id_waiting_for_reply; // defined in udp_io.c

char *str_state(State state) {
    switch (state) {
    case STATE_START:
        return "STATE_START";
    case STATE_AUTH:
        return "STATE_AUTH";
    case STATE_OPEN:
        return "STATE_OPEN";
    }
    return "";
}

bool is_valid_reply(Message message) {
    if (config.transport_type == Tcp) {
        return true;
    } else {
        // in UDP, we need to validate that REPLY's ref_message_id
        // refers to our original AUTH/JOIN's message_id
        return id_waiting_for_reply == message.ref_message_id;
    }
}

void handle_fsm_termination(bool send_messages) {
    switch (fsm_termination.kind) {
    case FSM_TERMINATION_NONE:
        break;

    case FSM_TERMINATION_SILENT:
        close_connection();
        sgc_free_all();
        exit(EXIT_SUCCESS);

    case FSM_TERMINATION_BYE:
        if (send_messages) {
            send_bye();

            if (config.transport_type == Udp) {
                wait_until_sent();
            }
        }

        close_connection();
        sgc_free_all();
        exit(EXIT_SUCCESS);

    case FSM_TERMINATION_ERROR:
        eprintf(fsm_termination.error_message);

        if (send_messages) {
            send_err(fsm_termination.error_message);
            if (config.transport_type == Udp) {
                wait_until_sent();
            }

            send_bye();
            if (config.transport_type == Udp) {
                wait_until_sent();
            }
        }

        close_connection();
        sgc_free_all();
        exit(EXIT_FAILURE);
    }
}
