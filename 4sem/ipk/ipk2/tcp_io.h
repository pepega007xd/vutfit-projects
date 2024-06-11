#ifndef TCP_IO_H
#define TCP_IO_H

#include "imports.h"
#include "message.h"
#include "user.h"

/// send message using TCP, message content
/// has to be valid only during this call,
/// returns true on success, false on failure
bool send_tcp_message(User *user, Message message);

/// Receives a message from user's socket
MessageResult recv_tcp_message(User *user);

#endif // !TCP_IO_H
