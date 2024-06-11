#include "udp_io.h"
#include "config.h"
#include "connection.h"
#include "imports.h"
#include "message.h"
#include "state_machine_utils.h"
#include "utils.h"
#include "vec.h"

extern Config config;
extern Connection connection;
extern FSMTermination fsm_termination;

// vector of messages not yet confirmed by server
GEN_VEC(UnconfirmedMessage, unconfirmed_message)
UnconfirmedMessageVec unconfirmed;

// message_id of latest message waiting for REPLY (either AUTH, or JOIN),
// updated only here
u16 id_waiting_for_reply;

// vector of received messsage IDs
GEN_VEC_H(u16, u16)
GEN_VEC(u16, u16)
GEN_VEC_COMPARE(u16, u16)
u16Vec received_message_ids;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// returns true, if provided message was previously received
bool is_already_received(Message message) {
    return u16_vec_find(received_message_ids, message.message_id, 0) != -1;
}

// adds message's ID to list of received IDs
void add_to_received(Message message) {
    u16_vec_push(&received_message_ids, message.message_id);
}

// get current time in millis
i64 get_time_ms(void) {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000 + now.tv_usec / 1000;
}

void send_udp_inner(u8Vec raw_message) {
    if (sendto(connection.server_socket_fd, raw_message.content,
               raw_message.size, 0, (struct sockaddr *)&config.address,
               sizeof(config.address)) != (i64)raw_message.size) {
        print_exit("UDP send failed: %s", strerror(errno));
    }
}

// returns a unique message id
u16 get_new_message_id(void) {
    static u16 message_id;
    return message_id++;
}

void send_udp_message(Message message) {
    message.message_id = get_new_message_id();
    if (message.message_type == MESSAGE_AUTH ||
        message.message_type == MESSAGE_JOIN) {
        id_waiting_for_reply = message.message_id;
    }

    u8Vec raw_message = serialize_udp_message(message);

    MessageOption new_message = parse_udp_message(raw_message);
    if (!new_message.is_valid) {
        print_exit("send_udp_message: invalid message passed as input");
    }

    UnconfirmedMessage unconfirmed_message = {
        .retries = config.udp_max_retransmissions,
        .message = new_message.content,
        .send_time_ms = get_time_ms()};

    send_udp_inner(raw_message);
    unconfirmed_message_vec_push(&unconfirmed, unconfirmed_message);
}

// delete message with provided ID from vector of unconfirmed messages
void mark_as_confirmed(u16 message_id) {
    for (u64 i = 0; i < unconfirmed.size; i++) {
        if (unconfirmed.content[i].message.message_id == message_id) {
            free_message(unconfirmed.content[i].message);
            unconfirmed_message_vec_delete(&unconfirmed, i);
            return;
        }
    }
}

MessageOption receive_udp_message(void) {
    u8 message_bytes[MAX_MESSAGE_SIZE];
    struct sockaddr_in server_address;
    u32 address_length = sizeof(server_address);

    i64 result =
        recvfrom(connection.server_socket_fd, message_bytes, MAX_MESSAGE_SIZE,
                 0, (struct sockaddr *)&server_address, &address_length);

    if (result == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
        return message_option_none();
    } else if (result == -1) {
        print_exit("Failed to receive UDP message: %s", strerror(errno));
    }

    // message is not from the expected server
    if (address_length != sizeof(config.address) ||
        server_address.sin_addr.s_addr != config.address.sin_addr.s_addr) {
        return message_option_none();
    }

    // parse message
    u8Vec raw_message = u8_vec_new();
    u8_vec_append(&raw_message, message_bytes, (u64)result);
    MessageOption message = parse_udp_message(raw_message);

    if (!message.is_valid) {
        // if the invalid message contains enough data to send a CONFIRM, do it
        //
        // note that we must not use raw_message, since it was deallocated after
        // unsuccessful parsing
        if (result >= 3 && message_bytes[0] != MESSAGE_CONFIRM) {
            Message confirm = {.message_type = MESSAGE_CONFIRM,
                               .ref_message_id = from_net_bytes(
                                   message_bytes[1], message_bytes[2])};

            u8Vec serialized = serialize_udp_message(confirm);

            send_udp_inner(serialized);
            u8_vec_clear(&serialized);
        }

        fsm_termination.kind = FSM_TERMINATION_ERROR;
        fsm_termination.error_message = "Invalid UDP message";
        return message;
    }

    // if message is the REPLY we're waiting for, set the new port to the source
    // address of the REPLY message
    if (message.content.message_type == MESSAGE_REPLY &&
        message.content.ref_message_id == id_waiting_for_reply) {
        config.address.sin_port = server_address.sin_port;
    }

    // if message is of type CONFIRM, mark our own message as confirmed
    if (message.content.message_type == MESSAGE_CONFIRM) {
        mark_as_confirmed(message.content.message_id);
        free_message(message.content);
        return message_option_none();
    } else {
        // send CONFIRM to received message
        Message confirm = {.message_type = MESSAGE_CONFIRM,
                           .ref_message_id = message.content.message_id};

        u8Vec serialized = serialize_udp_message(confirm);

        send_udp_inner(serialized);
        u8_vec_clear(&serialized);
    }

    // if message was received twice, discard it
    if (is_already_received(message.content)) {
        free_message(message.content);
        return message_option_none();
    } else {
        add_to_received(message.content);
    }

    return message;
}

void resend_messages(void) {
    i64 now = get_time_ms();

    for (u64 i = 0; i < unconfirmed.size; i++) {
        if (now - unconfirmed.content[i].send_time_ms >=
            config.udp_confirm_timeout) {
            if (unconfirmed.content[i].retries == 0) {
                print_exit("Failed to send UDP message");
            }

            unconfirmed.content[i].retries--;
            unconfirmed.content[i].send_time_ms = get_time_ms();
            send_udp_inner(unconfirmed.content[i].message.raw_message);
        }
    }
}

i32 get_timeout(void) {
    if (unconfirmed.size == 0) {
        return -1;
    }

    i64 max_message_age = 0;
    i64 now = get_time_ms();

    for (u64 i = 0; i < unconfirmed.size; i++) {
        i64 message_age = now - unconfirmed.content[i].send_time_ms;
        max_message_age = MAX(max_message_age, message_age);
    }

    return MAX((i32)(config.udp_confirm_timeout - max_message_age), 0);
}

void wait_until_sent(void) {
    while (unconfirmed.size != 0) {
        i32 timeout = get_timeout();
        if (timeout < 0) {
            print_exit("internal: there are unconfirmed messages "
                       "but timeout is negative");
        }

        usleep((u32)timeout * 1000);
        receive_udp_message();

        resend_messages();
    }
}
