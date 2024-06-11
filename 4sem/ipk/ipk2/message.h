#ifndef UDP_MESSAGE_H
#define UDP_MESSAGE_H

#include "imports.h"
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
        u8Vec raw_message;
        MessageType message_type;
        u16 message_id;     // UDP only, filled by send_udp_message
        u16 ref_message_id; // UDP only
        bool reply_result;
        char *username;        // max 20 bytes
        char *channel_id;      // max 20 bytes
        char *secret;          // max 128 bytes
        char *display_name;    // max 20 bytes
        char *message_content; // max 1400 bytes
} Message;

typedef enum {
    RESULT_NONE,             // received only a part of message
    RESULT_USER_ERROR,       // received malformed message
    RESULT_CONNECTION_ERROR, // could not receive message (connection is closed)
    RESULT_OK                // successfully received a message
} MessageResultVariant;

/// Type which represents the result of receiving a message,
/// field `message` is valid only in variant `RESULT_OK`
typedef struct {
        MessageResultVariant variant;
        Message message; // valid only in RESULT_OK
} MessageResult;

/// parses a message from raw bytes in vector, returns variant `RESULT_OK` with
/// the vector inside the `Message` struct if parsing succeeds, or
/// `RESULT_USER_ERROR` variant if the parsing fails, then the buffer
/// `raw_message` is deallocated
MessageResult parse_udp_message(u8Vec raw_message);

/// serializes `Messsage` struct into a raw message, copies all data =>
/// pointers in `message` need to be valid only during this function call
u8Vec serialize_udp_message(Message message);

/// same as udp parsing
MessageResult parse_tcp_message(u8Vec raw_message);

/// same as udp serialization
u8Vec serialize_tcp_message(Message message);

/// logs received message to stdout as specified by the assignment,
/// any other varints than `RESULT_OK` are skipped
void log_recv_message(struct sockaddr_in address, MessageResult message);

/// logs the sent message to stdout as specified by the assignment
void log_sent_message(struct sockaddr_in address, Message message);

/// deallocates the internal buffer
void free_message(Message message);

#endif // !UDP_MESSAGE_H
