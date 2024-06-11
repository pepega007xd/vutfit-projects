#ifndef UDP_IO_H
#define UDP_IO_H

#include "imports.h"
#include "message.h"
#include "user.h"

/// send message using UDP, message content has to be valid only during this
/// call, raw message is copied into internal buffer and later freed
bool send_udp_message(User *user, Message message);

/// Receives a message from user's socket, handles CONFIRM messages internally
MessageResult recv_udp_message(User *user);

/// Resend messages that have expired confirmation timeout
void resend_user_messages(User *user);

#endif // !UDP_IO_H
