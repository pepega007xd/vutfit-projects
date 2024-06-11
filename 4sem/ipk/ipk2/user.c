#include "user.h"
#include "config.h"
#include "connection.h"
#include "imports.h"
#include "message.h"
#include "tcp_io.h"
#include "udp_io.h"
#include "utils.h"
#include "vec.h"

extern Connection connection;
extern Config config;

GEN_VEC(User, user)

UserVec users;

char *server_display_name = "Server";
char *default_channel_name = "general";

#define MAX(a, b) ((a) > (b) ? (a) : (b))

bool user_already_logged_in(char *username) {
    for (u64 i = 0; i < users.size; i++) {
        if (users.content[i].user_socket == -1) {
            continue;
        }

        if (string_eq(users.content[i].username, username)) {
            return true;
        }
    }

    return false;
}

void send_message(User *user, Message message) {
    user->socket_type == TCP ? send_tcp_message(user, message)
                             : send_udp_message(user, message);
}

/// Send a message to a channel, `display_name` will be the name of the sender,
/// regardless of `original_sender`. `original_sender` is a User* which will be
/// skipped (if NULL, no user in that channel is skipped).
void channel_send_msg(char *channel, char *display_name, User *original_sender,
                      char *message_content) {
    for (u64 i = 0; i < users.size; i++) {
        User *user = &users.content[i];

        // skip deleted users
        if (user->user_socket == -1) {
            continue;
        }

        // skip the original sender
        if (original_sender != NULL &&
            string_eq(original_sender->username, user->username)) {
            continue;
        }

        if (user->user_state == STATE_OPEN &&
            string_eq(user->channel, channel)) {
            Message message = {.message_type = MESSAGE_MSG,
                               .display_name = display_name,
                               .message_content = message_content};

            send_message(user, message);
        }
    }
}

void join_user(User *user, char *new_channel) {
    // if user is already in this channel, do not send anything
    if (string_eq(user->channel, new_channel)) {
        return;
    }

    char old_channel[21] = {0};
    strcpy(old_channel, user->channel);

    // first move user to his new channel
    strcpy(user->channel, new_channel);

    // then send "user left" message to old channel (user must not see this)
    if (!string_is_empty(old_channel)) {
        char leave_message[64] = {0};
        snprintf(leave_message, 63, "%s has left %s.", user->display_name,
                 old_channel);
        channel_send_msg(old_channel, server_display_name, NULL, leave_message);
    }

    // finally send "user joined" message to new channel (user must see this)
    if (!string_is_empty(new_channel)) {
        char join_message[64] = {0};
        snprintf(join_message, 64, "%s has joined %s.", user->display_name,
                 new_channel);
        channel_send_msg(new_channel, server_display_name, NULL, join_message);
    }
}

void send_err(User *user, char *error_message) {
    Message message = {.message_type = MESSAGE_ERR,
                       .display_name = server_display_name,
                       .message_content = error_message};
    send_message(user, message);
}

void send_reply(User *user, Message orginal_message, bool is_positive,
                char *reply_message) {
    Message message = {.message_type = MESSAGE_REPLY,
                       .ref_message_id = orginal_message.message_id,
                       .reply_result = is_positive,
                       .message_content = reply_message};
    send_message(user, message);
}

void send_bye(User *user) {
    Message message = {.message_type = MESSAGE_BYE};
    send_message(user, message);
}

void handle_user_message_inner(User *user, MessageResult message_result) {
    if (message_result.variant == RESULT_NONE) {
        return;
    }

    if (message_result.variant == RESULT_CONNECTION_ERROR) {
        eprintf("lost connection with user %s", user->username);
        join_user(user, "");
        delete_user(user);
        return;
    }

    if (message_result.variant == RESULT_USER_ERROR) {
        eprintf("received invalid message from user %s", user->username);
        join_user(user, "");
        send_err(user, "Received invalid message");
        send_bye(user);
        user->user_state = STATE_END;
        return;
    }

    Message message = message_result.message;

    // update user's last display name
    if (message.message_type == MESSAGE_AUTH ||
        message.message_type == MESSAGE_JOIN ||
        message.message_type == MESSAGE_ERR ||
        message.message_type == MESSAGE_MSG) {
        strcpy(user->display_name, message.display_name);
    }

    switch (user->user_state) {
    case STATE_AUTH:
        if (message.message_type == MESSAGE_AUTH) {
            if (user_already_logged_in(message.username)) {
                send_reply(user, message, false,
                           "This username is already logged in");
            } else {
                send_reply(user, message, true, "Login successful");
                strcpy(user->username, message.username);
                user->user_state = STATE_OPEN;
                join_user(user, default_channel_name);
            }

        } else if (message.message_type == MESSAGE_BYE) {
            join_user(user, "");
            delete_user(user);
            return;

        } else {
            eprintf("received message invalid for current state from user %s",
                    user->username);
            send_err(user, "Cannot send this message when not authenticated");
            send_bye(user);
            user->user_state = STATE_END;
        }
        break;

    case STATE_OPEN:
        if (message.message_type == MESSAGE_MSG) {
            channel_send_msg(user->channel, user->display_name, user,
                             message.message_content);

        } else if (message.message_type == MESSAGE_JOIN) {
            // there is no reason why JOIN might fail
            send_reply(user, message, true, "Join successful");
            join_user(user, message.channel_id);

        } else if (message.message_type == MESSAGE_ERR) {
            join_user(user, "");
            send_bye(user);
            user->user_state = STATE_END;

        } else if (message.message_type == MESSAGE_BYE) {
            join_user(user, "");
            delete_user(user);
            return;

        } else {
            eprintf("received message invalid for current state from user %s",
                    user->username);
            send_err(user, "Cannot send this message when authenticated");
            send_bye(user);
            user->user_state = STATE_END;
        }
        break;

    case STATE_END:
        // do not handle any incoming messages from user,
        // if they're already in state END
        break;
    }

    free_message(message);
}

void add_tcp_user(i32 user_socket, struct sockaddr_in user_address) {
    User *user = user_vec_push(&users, (User){.socket_type = TCP,
                                              .user_socket = user_socket,
                                              .user_address = user_address,
                                              .user_state = STATE_AUTH});

    MessageResult auth_message = recv_tcp_message(user);

    handle_user_message_inner(user, auth_message);
}

void add_udp_user(i32 user_socket) {
    // we first set user's socket to the auth socket,
    // to receive the AUTH message from user
    User *user =
        user_vec_push(&users, (User){.socket_type = UDP,
                                     .user_socket = connection.udp_auth_socket,
                                     .user_state = STATE_AUTH});

    MessageResult auth_message = recv_udp_message(user);

    // now we set the user's socket to the correct value
    user->user_socket = user_socket;

    handle_user_message_inner(user, auth_message);
}

void handle_user_message(i32 user_socket) {
    User *user = NULL;

    // find user corresponding to socket

    for (u64 i = 0; i < users.size; i++) {
        if (users.content[i].user_socket == user_socket) {
            user = &users.content[i];
        }
    }

    if (user == NULL) {
        eprintf("internal: socket doesn't correspond to any user");
        return;
    }

    MessageResult message_result = user->socket_type == TCP
                                       ? recv_tcp_message(user)
                                       : recv_udp_message(user);

    handle_user_message_inner(user, message_result);
}

void delete_users_in_end_state(void) {
    for (u64 i = 0; i < users.size; i++) {
        User *user = &users.content[i];

        // skip deleted users
        if (user->user_socket == -1) {
            continue;
        }

        // if the user is on TCP, we can delete him immediately
        if (user->user_state == STATE_END && user->socket_type == TCP) {
            delete_user(user);
        }

        // if the user is on UDP, we have to wait until
        // all sent messages are confirmed
        if (user->user_state == STATE_END &&
            user->unconfirmed_messages.size == 0) {
            delete_user(user);
        }
    }
}

void purge_deleted_users(void) {
    for (u64 i = 0; i < users.size;) {
        if (users.content[i].user_socket == -1) {
            user_vec_delete(&users, i);
        } else {
            i++;
        }
    }
}

void resend_udp_messages(void) {
    for (u64 i = 0; i < users.size; i++) {
        if (users.content[i].user_socket != -1) {
            resend_user_messages(&users.content[i]);
        }
    }
}

void delete_user(User *user) {
    // remove socket from epoll list
    epoll_ctl(connection.epoll_fd, EPOLL_CTL_DEL, user->user_socket, NULL);

    close(user->user_socket);

    u16_vec_clear(&user->received_message_ids);
    unconfirmed_message_vec_clear(&user->unconfirmed_messages);
    u8_vec_clear(&user->message_buffer);

    // invalidate user data
    user->user_socket = -1;
    user->channel[0] = '\0';
    user->username[0] = '\0';
    user->user_state = STATE_END;
}

i32 get_timeout(void) {
    i64 max_message_age = 0;
    bool any_unconfirmed_messages_present = false;
    for (u64 user_idx = 0; user_idx < users.size; user_idx++) {
        User user = users.content[user_idx];

        if (user.user_socket == -1) {
            continue;
        }

        i64 now = get_time_ms();

        if (user.unconfirmed_messages.size > 0) {
            any_unconfirmed_messages_present = true;
        }

        for (u64 unconfirmed_idx = 0;
             unconfirmed_idx < user.unconfirmed_messages.size;
             unconfirmed_idx++) {
            i64 message_age =
                now -
                user.unconfirmed_messages.content[unconfirmed_idx].send_time_ms;

            max_message_age = MAX(max_message_age, message_age);
        }
    }

    if (!any_unconfirmed_messages_present) {
        return -1;
    }

    return MAX((i32)(config.udp_confirm_timeout - max_message_age), 0);
}
