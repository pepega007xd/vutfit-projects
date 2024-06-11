#ifndef UDP_MESSAGE_H
#define UDP_MESSAGE_H

#include "imports.h"
#include "option.h"
#include "vec.h"

typedef enum {
    MESSAGE_CONFIRM = 0x0,
    MESSAGE_REPLY = 0x1,
    MESSAGE_AUTH = 0x2,
    MESSAGE_JOIN = 0x3,
    MESSAGE_MSG = 0x4,
    MESSAGE_ERR = 0xfe,
    MESSAGE_BYE = 0xff,
} MessageType;

/// this structure is used for both TCP and UDP messages,
/// fields that are unused in TCP are simply left uninitialized/zeroed when
/// creating a TCP message, since they are not read in TCP message handling code
typedef struct {
        // all `char *` fields of this struct point to this buffer,
        // it needs to be deallocated when destroying a message
        u8Vec raw_message;

        MessageType message_type;
        u16 message_id;        // UDP only, filled out by send_udp_message
        u16 ref_message_id;    // UDP only
        bool reply_result;     // true means success, false means failure
        char *username;        // max 20 bytes
        char *channel_id;      // max 20 bytes
        char *secret;          // max 128 bytes
        char *display_name;    // max 20 bytes
        char *message_content; // max 1400 bytes
} Message;

GEN_OPTION_H(Message, message)

/// parses a UDP message from raw bytes in vector, returns Message struct with
/// the vector inside the struct if parsing succeeds, in case of failure, the
/// buffer `raw_message` is deallocated
MessageOption parse_udp_message(u8Vec raw_message);

/// serializes Messsage struct into a UDP raw message, copies all data =>
/// pointers in Message need to be valid only during the lifetime of this
/// function
u8Vec serialize_udp_message(Message message);

/// same as `parse_udp_message`, except for TCP message
MessageOption parse_tcp_message(u8Vec raw_message);

/// same as `serialize_udp_message`, except for TCP message
u8Vec serialize_tcp_message(Message message);

/// prints message to stdout/stderr as required by the assignment
void print_message(Message message);

/// deallocates the vector inside message
void free_message(Message message);

#endif // !UDP_MESSAGE_H
