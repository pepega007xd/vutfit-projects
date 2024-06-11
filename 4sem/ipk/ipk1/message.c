#include "message.h"
#include "vec.h"

void print_message(Message message) {
    switch (message.message_type) {
    case MESSAGE_REPLY:
        fprintf(stderr, "%s: %s\n",
                message.reply_result ? "Success" : "Failure",
                message.message_content);
        break;

    case MESSAGE_MSG:
        fprintf(stdout, "%s: %s\n", message.display_name,
                message.message_content);
        break;

    case MESSAGE_ERR:
        fprintf(stderr, "ERR FROM %s: %s\n", message.display_name,
                message.message_content);
        break;

    default:
        break;
    }
}

void free_message(Message message) { u8_vec_clear(&message.raw_message); }
