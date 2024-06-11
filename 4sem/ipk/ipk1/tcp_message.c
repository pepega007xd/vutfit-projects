#include "message.h"
#include "option.h"
#include "utils.h"
#include "validation.h"
#include "vec.h"

GEN_OPTION(Message, message)

#define parse_string(name, length, compare_function)                           \
    message.name = strtok(NULL, " ");                                          \
    if (message.name == NULL) {                                                \
        goto ERR_INVALID_MESSAGE;                                              \
    }                                                                          \
    if (validate_content(raw_message,                                          \
                         (i32)(message.name - (char *)raw_message.content),    \
                         length, compare_function) == -1) {                    \
        goto ERR_INVALID_MESSAGE;                                              \
    }

#define parse_keyword(keyword_literal)                                         \
    keyword = strtok(NULL, " ");                                               \
    if (keyword == NULL) {                                                     \
        goto ERR_INVALID_MESSAGE;                                              \
    }                                                                          \
    if (!string_eq_nocase(keyword, keyword_literal)) {                         \
        goto ERR_INVALID_MESSAGE;                                              \
    }

#define parse_content()                                                        \
    message.message_content = strtok(NULL, "");                                \
    if (message.message_content == NULL) {                                     \
        goto ERR_INVALID_MESSAGE;                                              \
    }                                                                          \
    if (validate_content(                                                      \
            raw_message,                                                       \
            (i32)(message.message_content - (char *)raw_message.content),      \
            1400, is_printable) == -1) {                                       \
        goto ERR_INVALID_MESSAGE;                                              \
    }

#define assert_end()                                                           \
    {                                                                          \
        char *next_token = strtok(NULL, "");                                   \
        if (next_token != NULL) {                                              \
            goto ERR_INVALID_MESSAGE;                                          \
        }                                                                      \
    }

MessageOption parse_tcp_message(u8Vec raw_message) {
    {
        Message message = {.raw_message = raw_message,
                           .reply_result = false,
                           .username = NULL,
                           .channel_id = NULL,
                           .secret = NULL,
                           .display_name = NULL,
                           .message_content = NULL};

        // find \r\n ending, validate it, and replace \r with \0 byte
        i32 end = u8_vec_find(raw_message, '\r', 0);

        if (end == -1 || end != (i32)raw_message.size - 2 ||
            raw_message.content[end + 1] != '\n') {
            goto ERR_INVALID_MESSAGE;
        }

        if (!is_printable_string(raw_message)) {
            goto ERR_INVALID_MESSAGE;
        }

        raw_message.content[end] = '\0';

        char *keyword = strtok((char *)raw_message.content, " ");
        if (keyword == NULL) {
            goto ERR_INVALID_MESSAGE;
        }

        if (string_eq_nocase(keyword, "AUTH")) {
            message.message_type = MESSAGE_AUTH;
            parse_string(username, 20, is_alphanumdash);
            parse_keyword("AS");
            parse_string(display_name, 20, is_printable_nospace);
            parse_keyword("USING");
            parse_string(secret, 128, is_alphanumdash);
            assert_end();

        } else if (string_eq_nocase(keyword, "JOIN")) {
            message.message_type = MESSAGE_JOIN;
            parse_string(channel_id, 20, is_alphanumdash);
            parse_keyword("AS");
            parse_string(display_name, 20, is_printable_nospace);
            assert_end();

        } else if (string_eq_nocase(keyword, "MSG")) {
            message.message_type = MESSAGE_MSG;
            parse_keyword("FROM");
            parse_string(display_name, 20, is_printable_nospace);
            parse_keyword("IS");
            parse_content();
            assert_end();

        } else if (string_eq_nocase(keyword, "ERR")) {
            message.message_type = MESSAGE_ERR;
            parse_keyword("FROM");
            parse_string(display_name, 20, is_printable_nospace);
            parse_keyword("IS");
            parse_content();
            assert_end();

        } else if (string_eq_nocase(keyword, "REPLY")) {
            message.message_type = MESSAGE_REPLY;

            // have to parse OK or NOK manually
            keyword = strtok(((void *)0), " ");
            if (keyword == NULL) {
                goto ERR_INVALID_MESSAGE;
            }
            if (string_eq_nocase(keyword, "OK")) {
                message.reply_result = true;
            } else if (string_eq_nocase(keyword, "NOK")) {
                message.reply_result = false;
            } else {
                goto ERR_INVALID_MESSAGE;
            }

            parse_keyword("IS");
            parse_content();
            assert_end();

        } else if (string_eq_nocase(keyword, "BYE")) {
            message.message_type = MESSAGE_BYE;
            assert_end();

        } else {
            goto ERR_INVALID_MESSAGE;
        }

        return message_option_some(message);
    }

ERR_INVALID_MESSAGE:
    u8_vec_clear(&raw_message);
    return message_option_none();
}

u8Vec serialize_tcp_message(Message message) {
    u8Vec raw_message = u8_vec_new();

    switch (message.message_type) {
    case MESSAGE_JOIN:
        append_string(&raw_message, "JOIN ");
        append_string(&raw_message, message.channel_id);
        append_string(&raw_message, " AS ");
        append_string(&raw_message, message.display_name);
        break;

    case MESSAGE_MSG:
        append_string(&raw_message, "MSG FROM ");
        append_string(&raw_message, message.display_name);
        append_string(&raw_message, " IS ");
        append_string(&raw_message, message.message_content);
        break;

    case MESSAGE_AUTH:
        append_string(&raw_message, "AUTH ");
        append_string(&raw_message, message.username);
        append_string(&raw_message, " AS ");
        append_string(&raw_message, message.display_name);
        append_string(&raw_message, " USING ");
        append_string(&raw_message, message.secret);
        break;

    case MESSAGE_ERR:
        append_string(&raw_message, "ERR FROM ");
        append_string(&raw_message, message.display_name);
        append_string(&raw_message, " IS ");
        append_string(&raw_message, message.message_content);
        break;

    case MESSAGE_REPLY:
        message.reply_result ? append_string(&raw_message, "REPLY OK IS ")
                             : append_string(&raw_message, "REPLY NOK IS ");
        append_string(&raw_message, message.message_content);
        break;

    case MESSAGE_BYE:
        append_string(&raw_message, "BYE");
        break;

    case MESSAGE_CONFIRM:
        print_exit("internal: CONFIRM message cannot appear in TCP");
    }

    u8_vec_push(&raw_message, '\r');
    u8_vec_push(&raw_message, '\n');

    return raw_message;
}
