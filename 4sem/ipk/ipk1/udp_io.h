#ifndef UDP_IO_H
#define UDP_IO_H

#include "imports.h"
#include "message.h"
#include "vec.h"

typedef struct {
        Message message;
        i64 send_time_ms;
        u8 retries;
} UnconfirmedMessage;

GEN_VEC_H(UnconfirmedMessage, unconfirmed_message)

/// calculates the timeout for `poll`, a maximum value we can sleep before we
/// must handle a UDP retransmission
i32 get_timeout(void);

/// send message using UDP, message content has to be valid only during this
/// call, content is copied into internal buffer
void send_udp_message(Message message);

/// Receives message using UDP, 'none' indicates that
/// the message was UDP-specific and was processed internally
MessageOption receive_udp_message(void);

/// Resend messages that have expired confirmation timeout
void resend_messages(void);

/// Waits until all currently sent messages are confirmed
void wait_until_sent(void);
#endif // !UDP_IO_H
