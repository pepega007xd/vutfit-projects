#include "connection.h"
#include "imports.h"
#include "message.h"
#include "state_machine_utils.h"
#include "utils.h"
#include "vec.h"

extern Connection connection;
extern FSMTermination fsm_termination;

void send_tcp_message(Message message) {
    u8Vec raw_message = serialize_tcp_message(message);

    if (write(connection.server_socket_fd, raw_message.content,
              raw_message.size) != (i64)raw_message.size) {
        print_exit("Failed to send TCP message");
    }

    u8_vec_clear(&raw_message);
}

MessageOption receive_tcp_message(void) {
    static u8Vec message_buffer;

    // read bytes from socket up to '\n' - single message
    while (true) {
        u8 message_byte;
        i64 size = read(connection.server_socket_fd, &message_byte, 1);

        if (size == 0) {
            print_exit("disconnected");

        } else if (size == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
            return message_option_none();

        } else if (size == -1) {
            print_exit("Failed to receive TCP message: %s", strerror(errno));
        } else {
            u8_vec_push(&message_buffer, message_byte);
            if (message_byte == '\n') {
                break;
            }
        }
    }

    // parse message
    MessageOption message = parse_tcp_message(message_buffer);
    if (!message.is_valid) {
        fsm_termination.kind = FSM_TERMINATION_ERROR;
        fsm_termination.error_message = "Invalid message from TCP";
    }

    // zero out message buffer
    message_buffer = (u8Vec){0};

    return message;
}
