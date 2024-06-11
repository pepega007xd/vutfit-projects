#ifndef USER_H
#define USER_H

#include "imports.h"
#include "vec.h"

/// Type representing a UDP message sent to the client
typedef struct {
        u8Vec raw_message;
        u16 message_id;
        i64 send_time_ms; // returned by get_time_ms
        u8 retries;       // number of available retries
} UnconfirmedMessage;

GEN_VEC_H(UnconfirmedMessage, unconfirmed_message)

typedef enum { TCP, UDP } SocketType;

/// state ERROR is not represented in the FSM,
/// since it immediately transitions to END while sending a BYE message,
/// this is done in the code directly
typedef enum { STATE_AUTH, STATE_OPEN, STATE_END } UserState;

/// This struct represents an active connection with a user
typedef struct {
        SocketType socket_type;
        i32 user_socket;
        struct sockaddr_in user_address;
        UserState user_state;
        char username[21];
        char display_name[21];
        char channel[21];
        u16Vec received_message_ids;                // UDP only
        UnconfirmedMessageVec unconfirmed_messages; // UDP only
        u8Vec message_buffer;                       // TCP only
} User;

GEN_VEC_H(User, user)

/// Creates a new TCP user and receives an AUTH message,
/// `user_address` is address returned by `accept`
void add_tcp_user(i32 user_socket, struct sockaddr_in user_address);

/// Creates a new UDP user and receives an AUTH message,
/// user's address is configured according to their first message
void add_udp_user(i32 user_socket);

/// Process message from an already connected client
void handle_user_message(i32 user_socket);

/// Resend UDP messages that have expired confirmation timeout
void resend_udp_messages(void);

/// Deletes all associated data structures of the user, but does not immediately
/// remove the user from the `users` vector => this function is safe to call in
/// a loop over `users`
void delete_user(User *);

/// Deletes users which are in END state (disconnected), for UDP users,
/// this functions checks for unconfirmed messages
void delete_users_in_end_state(void);

/// Actually removes the invalidated user structs from the `users` vector
void purge_deleted_users(void);

/// calculates the timeout for epoll, a maximum value in milliseconds we can
/// sleep before we must handle a UDP retransmission, -1 is returned if there
/// are no unconfirmed messages
i32 get_timeout(void);

#endif // !USER_H
