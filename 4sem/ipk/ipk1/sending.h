#ifndef SENDING_H
#define SENDING_H

#include "user_input.h"

/// transport-independent abstractions for sending messages to server

/// Sends message corresponding to the provided user_input
void send_user_input(UserInput user_input);

/// Sends an ERR message
void send_err(char *message_content);

/// Sends a BYE message
void send_bye(void);

#endif // !SENDING_H
