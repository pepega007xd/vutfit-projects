#include "imports.h"
#include "message.h"
#include "user.h"
#include "utils.h"
#include "vec.h"

bool send_tcp_message(User *user, Message message) {
    log_sent_message(user->user_address, message);

    u8Vec raw_message = serialize_tcp_message(message);

    bool result = write(user->user_socket, raw_message.content,
                        raw_message.size) == (i64)raw_message.size;

    if (!result) {
        eprintf("Failed to send TCP message");
    }

    u8_vec_clear(&raw_message);

    return result;
}

MessageResult recv_tcp_message(User *user) {
    // read bytes from socket up to '\n' - single message
    while (true) {
        u8 message_byte;
        i64 size = read(user->user_socket, &message_byte, 1);

        if (size == 0) {
            eprintf("TCP user disconnected");
            return (MessageResult){.variant = RESULT_CONNECTION_ERROR};

        } else if (size == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
            return (MessageResult){.variant = RESULT_NONE};

        } else if (size == -1) {
            eprintf("Failed to receive TCP message: %s", strerror(errno));
            return (MessageResult){.variant = RESULT_CONNECTION_ERROR};

        } else {
            u8_vec_push(&user->message_buffer, message_byte);
            if (message_byte == '\n') {
                break;
            }
        }
    }

    // parse message
    MessageResult message = parse_tcp_message(user->message_buffer);

    log_recv_message(user->user_address, message);

    // zero out message buffer
    user->message_buffer = (u8Vec){0};

    return message;
}
