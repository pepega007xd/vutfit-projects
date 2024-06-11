#include "imports.h"
#include "message.h"
#include "option.h"
#include "utils.h"
#include "validation.h"
#include "vec.h"

#define assert_min_len(len)                                                    \
    if (raw_message.size < len) {                                              \
        goto ERR_INVALID_MESSAGE;                                              \
    }

#define parse_string(name, start, length, compare_function)                    \
    message.name = (char *)raw_message.content + start;                        \
    end = validate_content(raw_message, start, length, compare_function);      \
    if (end == -1) {                                                           \
        goto ERR_INVALID_MESSAGE;                                              \
    }

#define assert_end()                                                           \
    if ((u64)end != raw_message.size - 1 || raw_message.content[end] != 0) {   \
        goto ERR_INVALID_MESSAGE;                                              \
    }

MessageOption parse_udp_message(u8Vec raw_message) {
    Message message = {.raw_message = raw_message,
                       .message_id = 0,
                       .ref_message_id = 0,
                       .reply_result = false,
                       .username = NULL,
                       .channel_id = NULL,
                       .secret = NULL,
                       .display_name = NULL,
                       .message_content = NULL};

    // size of smallest possible message - BYE
    // (1 byte message_type + 2 bytes message_id)
    assert_min_len(3);

    message.message_type = raw_message.content[0];
    message.message_id =
        from_net_bytes(raw_message.content[1], raw_message.content[2]);

    i32 end;
    switch (message.message_type) {
    case MESSAGE_CONFIRM:
        if (raw_message.size != 3) {
            goto ERR_INVALID_MESSAGE;
        }

        message.ref_message_id =
            from_net_bytes(raw_message.content[1], raw_message.content[2]);
        break;

    case MESSAGE_REPLY:
        assert_min_len(8);

        if (raw_message.content[3] != 0 && raw_message.content[3] != 1) {
            goto ERR_INVALID_MESSAGE;
        }
        message.reply_result = raw_message.content[3];

        message.ref_message_id =
            from_net_bytes(raw_message.content[4], raw_message.content[5]);

        parse_string(message_content, 6, 1400, is_printable);
        assert_end();
        break;

    case MESSAGE_AUTH:
        assert_min_len(9);
        parse_string(username, 3, 20, is_alphanumdash);
        parse_string(display_name, end + 1, 20, is_printable_nospace);
        parse_string(secret, end + 1, 128, is_alphanumdash);
        assert_end();
        break;

    case MESSAGE_JOIN:
        assert_min_len(7);
        parse_string(channel_id, 3, 20, is_alphanumdash);
        parse_string(display_name, end + 1, 20, is_printable_nospace);
        assert_end();
        break;

    case MESSAGE_MSG:
        assert_min_len(7);
        parse_string(display_name, 3, 20, is_printable_nospace);
        parse_string(message_content, end + 1, 1400, is_printable);
        assert_end();
        break;

    case MESSAGE_ERR:
        assert_min_len(7);
        parse_string(display_name, 3, 20, is_printable_nospace);
        parse_string(message_content, end + 1, 1400, is_printable);
        assert_end();
        break;

    case MESSAGE_BYE:
        if (raw_message.size != 3) {
            goto ERR_INVALID_MESSAGE;
        }
        break;

    default:
        goto ERR_INVALID_MESSAGE;
    }

    return message_option_some(message);

ERR_INVALID_MESSAGE:
    u8_vec_clear(&raw_message);
    return message_option_none();
}

void push_u16(u8Vec *vector, u16 data) {
    u8 *data_bytes = (u8 *)&data;
    u8_vec_push(vector, data_bytes[0]);
    u8_vec_push(vector, data_bytes[1]);
}

void append_string_nullbyte(u8Vec *vector, char *string) {
    append_string(vector, string);
    u8_vec_push(vector, 0);
}

u8Vec serialize_udp_message(Message message) {
    u8Vec raw_message = u8_vec_new();
    u8_vec_push(&raw_message, (u8)message.message_type);

    message.message_type == MESSAGE_CONFIRM
        ? push_u16(&raw_message, htons(message.ref_message_id))
        : push_u16(&raw_message, htons(message.message_id));

    switch (message.message_type) {
    case MESSAGE_CONFIRM:
        break;

    case MESSAGE_REPLY:
        u8_vec_push(&raw_message, message.reply_result);
        push_u16(&raw_message, message.ref_message_id);
        append_string_nullbyte(&raw_message, message.message_content);
        break;

    case MESSAGE_AUTH:
        append_string_nullbyte(&raw_message, message.username);
        append_string_nullbyte(&raw_message, message.display_name);
        append_string_nullbyte(&raw_message, message.secret);
        break;

    case MESSAGE_JOIN:
        append_string_nullbyte(&raw_message, message.channel_id);
        append_string_nullbyte(&raw_message, message.display_name);
        break;

    case MESSAGE_MSG:
        append_string_nullbyte(&raw_message, message.display_name);
        append_string_nullbyte(&raw_message, message.message_content);
        break;

    case MESSAGE_ERR:
        append_string_nullbyte(&raw_message, message.display_name);
        append_string_nullbyte(&raw_message, message.message_content);
        break;

    case MESSAGE_BYE:
        break;
    }

    return raw_message;
}
