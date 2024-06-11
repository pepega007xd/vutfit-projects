#ifndef TCP_IO_H
#define TCP_IO_H

#include "imports.h"
#include "message.h"

/// send message using TCP, message content has to be valid
/// only during this call
void send_tcp_message(Message message);

/// Receives message using TCP, 'none' indicates that
/// the whole message has not yet been received
MessageOption receive_tcp_message(void);

#endif // !TCP_IO_H
