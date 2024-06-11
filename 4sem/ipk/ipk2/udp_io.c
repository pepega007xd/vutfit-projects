#include "udp_io.h"
#include "config.h"
#include "imports.h"
#include "message.h"
#include "user.h"
#include "utils.h"
#include "vec.h"

extern Config config;

GEN_VEC(UnconfirmedMessage, unconfirmed_message)

/// sends raw_message to user, returns false on error
bool send_udp_inner(User *user, u8Vec raw_message) {
    // log sent message to stdout
    MessageResult message = parse_udp_message(raw_message);
    log_sent_message(user->user_address, message.message);

    if (sendto(user->user_socket, raw_message.content, raw_message.size, 0,
               (struct sockaddr *)&user->user_address,
               sizeof(user->user_address)) != (i64)raw_message.size) {
        eprintf("UDP send failed: %s", strerror(errno));
        return false;
    }
    return true;
}

u16 get_new_message_id(void) {
    static u16 message_id;
    return message_id++;
}

bool send_udp_message(User *user, Message message) {
    message.message_id = get_new_message_id();

    u8Vec raw_message = serialize_udp_message(message);

    if (!send_udp_inner(user, raw_message)) {
        eprintf("Failed to send UDP message");
        return false;
    }

    unconfirmed_message_vec_push(
        &user->unconfirmed_messages,
        (UnconfirmedMessage){.raw_message = raw_message,
                             .message_id = message.message_id,
                             .retries = config.udp_max_retransmissions,
                             .send_time_ms = get_time_ms()});

    return true;
}

/// marks message with `ref_message_id` as confirmed by the client
void confirm_message(User *user, u16 ref_message_id) {
    for (u64 i = 0; i < user->unconfirmed_messages.size; i++) {
        if (user->unconfirmed_messages.content[i].message_id ==
            ref_message_id) {
            u8_vec_clear(&user->unconfirmed_messages.content[i].raw_message);
            unconfirmed_message_vec_delete(&user->unconfirmed_messages, i);
            return;
        }
    }
    eprintf("confirm message not associated with any unconfirmed message");
}

/// returns true if the message was received earlier (client might not have
/// received the confirm, and is therefore sending the message again)
bool is_already_received(User *user, Message message) {
    return u16_vec_find(user->received_message_ids, message.message_id, 0) !=
           -1;
}

/// adds the message's id into the list of ids received from the user
void add_to_received(User *user, Message message) {
    u16_vec_push(&user->received_message_ids, message.message_id);
}

MessageResult recv_udp_message(User *user) {
    u8 buffer[BUFFER_SIZE] = {0};

    // receive packet from socket

    struct sockaddr_in user_address;
    u32 address_length = sizeof(user->user_address);
    i64 result = recvfrom(user->user_socket, buffer, BUFFER_SIZE, 0,
                          (struct sockaddr *)&user_address, &address_length);

    if (result == -1) {
        eprintf("Failed to receive UDP message: %s", strerror(errno));
        return (MessageResult){.variant = RESULT_CONNECTION_ERROR};
    }

    // check the source address, or if the client is new, update their address

    if (user->user_state == STATE_AUTH) {
        // set new client's address
        user->user_address = user_address;
    } else if (user->user_address.sin_addr.s_addr !=
                   user_address.sin_addr.s_addr ||
               user->user_address.sin_port != user_address.sin_port) {
        // received message is not from the expected source address
        eprintf("Received UDP message from unexpected address");
        return (MessageResult){.variant = RESULT_NONE};
    }

    // copy packet content to buffer

    u8Vec raw_message = u8_vec_new();
    u8_vec_append(&raw_message, buffer, (u64)result);

    // parse packet as UDP message

    MessageResult message_result = parse_udp_message(raw_message);
    log_recv_message(user->user_address, message_result);

    if (message_result.variant == RESULT_USER_ERROR ||
        message_result.variant == RESULT_NONE) {
        return message_result;
    }

    if (message_result.message.message_type == MESSAGE_CONFIRM) {
        // mark our message as confirmed
        confirm_message(user, message_result.message.ref_message_id);
        free_message(message_result.message);
        return (MessageResult){.variant = RESULT_NONE};

    } else {
        // send CONFIRM to received message
        Message confirm = {.message_type = MESSAGE_CONFIRM,
                           .ref_message_id = message_result.message.message_id};

        u8Vec serialized = serialize_udp_message(confirm);

        send_udp_inner(user, serialized);
        u8_vec_clear(&serialized);
    }

    // if message was received twice, discard it
    if (is_already_received(user, message_result.message)) {
        free_message(message_result.message);
        return (MessageResult){.variant = RESULT_NONE};
    } else {
        add_to_received(user, message_result.message);
    }

    return message_result;
}

void resend_user_messages(User *user) {
    i64 now = get_time_ms();
    for (u64 i = 0; i < user->unconfirmed_messages.size; i++) {
        UnconfirmedMessage *unconfirmed =
            &user->unconfirmed_messages.content[i];

        if (now - unconfirmed->send_time_ms >= config.udp_confirm_timeout) {
            if (unconfirmed->retries == 0) {
                eprintf("Maximum UDP retransmit count reached for user %s",
                        user->username);
                delete_user(user);
                return;
            }

            unconfirmed->retries--;
            unconfirmed->send_time_ms = get_time_ms();
            send_udp_inner(user, unconfirmed->raw_message);
        }
    }
}
