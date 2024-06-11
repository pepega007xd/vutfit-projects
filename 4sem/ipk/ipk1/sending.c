#include "config.h"
#include "message.h"
#include "tcp_io.h"
#include "udp_io.h"
#include "user_input.h"

extern Config config;
extern char display_name[21];

void send_inner(Message message) {
    config.transport_type == Tcp ? send_tcp_message(message)
                                 : send_udp_message(message);
}

void send_user_input(UserInput user_input) {
    Message message;

    // all messages have a display name
    message.display_name = display_name;

    switch (user_input.command_type) {
    case COMMAND_MSG:
        message.message_type = MESSAGE_MSG;
        message.message_content = user_input.message;
        break;

    case COMMAND_AUTH:
        message.message_type = MESSAGE_AUTH;
        message.username = user_input.username;
        message.secret = user_input.secret;
        break;

    case COMMAND_JOIN:
        message.message_type = MESSAGE_JOIN;
        message.channel_id = user_input.channel_id;
        break;

    case COMMAND_RENAME:
        // RENAME is not sent to server
        return;
    }

    send_inner(message);
}

void send_err(char *message_content) {
    send_inner((Message){.message_type = MESSAGE_ERR,
                         .display_name = display_name,
                         .message_content = message_content});
}

void send_bye(void) { send_inner((Message){.message_type = MESSAGE_BYE}); }
