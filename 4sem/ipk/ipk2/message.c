#include "message.h"
#include "vec.h"

void print_message(Message message) {
    switch (message.message_type) {
    case MESSAGE_CONFIRM:
        printf("CONFIRM");
        break;

    case MESSAGE_REPLY:
        printf("REPLY Result=%s MessageContents=%s",
               message.reply_result ? "OK" : "NOK", message.message_content);
        break;

    case MESSAGE_AUTH:
        printf("AUTH Username=%s DisplayName=%s Secret=%s", message.username,
               message.display_name, message.secret);
        break;

    case MESSAGE_JOIN:
        printf("JOIN ChannelID=%s DisplayName=%s", message.channel_id,
               message.display_name);
        break;

    case MESSAGE_MSG:
        printf("MSG DisplayName=%s MessageContents=%s", message.display_name,
               message.message_content);
        break;

    case MESSAGE_ERR:
        printf("ERR DisplayName=%s MessageContents=%s", message.display_name,
               message.message_content);
        break;

    case MESSAGE_BYE:
        printf("BYE");
        break;
    }

    printf("\n");
}

void log_message(struct sockaddr_in address, Message message, char *type) {
    char address_buffer[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &address.sin_addr, address_buffer, INET_ADDRSTRLEN);

    printf("%s %s:%u | ", type, address_buffer, ntohs(address.sin_port));

    print_message(message);
}

void log_recv_message(struct sockaddr_in address, MessageResult message) {
    // skip incomplete/invalid messages and connection failures
    if (message.variant != RESULT_OK) {
        return;
    }

    log_message(address, message.message, "RECV");
}

void log_sent_message(struct sockaddr_in address, Message message) {
    log_message(address, message, "SENT");
}

void free_message(Message message) { u8_vec_clear(&message.raw_message); }
