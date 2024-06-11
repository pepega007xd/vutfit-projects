#include "event_loop.h"
#include "connection.h"
#include "user.h"
#include "utils.h"

extern Connection connection;

/// add socket to epoll interest list, and set it as nonblocking,
/// returns true when successful, false on failure
bool configure_socket(i32 user_socket) {
    struct epoll_event event = {.data.fd = user_socket, .events = EPOLLIN};

    if (epoll_ctl(connection.epoll_fd, EPOLL_CTL_ADD, user_socket, &event) ==
        -1) {
        eprintf("Failed to add socket to interest list: %s", strerror(errno));
        close(user_socket);
        return false;
    }

    i32 flags = fcntl(user_socket, F_GETFL, 0);
    if (flags == -1) {
        eprintf("Failed to get socket flags: %s", strerror(errno));
        return false;
    }
    if (fcntl(user_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        eprintf("Failed to get socket flags: %s", strerror(errno));
        return false;
    }

    return true;
}

void run_event_loop(void) {

    while (true) {
        i32 timeout = get_timeout();

        struct epoll_event input_event;
        i32 available =
            epoll_wait(connection.epoll_fd, &input_event, 1, timeout);

        if (available == -1) {
            // epoll failed
            print_exit("epoll_wait failed: %s", strerror(errno));

        } else if (available == 0) {
            // epoll timed out
            resend_udp_messages();

        } else if (input_event.data.fd == connection.tcp_auth_socket) {
            // new TCP user
            struct sockaddr_in user_address;
            u32 address_len = sizeof(struct sockaddr_in);
            i32 user_socket =
                accept(connection.tcp_auth_socket,
                       (struct sockaddr *)&user_address, &address_len);

            if (user_socket == -1) {
                eprintf("accept failed: %s", strerror(errno));
                continue;
            }

            if (!configure_socket(user_socket)) {
                close(user_socket);
                continue;
            }

            add_tcp_user(user_socket, user_address);

        } else if (input_event.data.fd == connection.udp_auth_socket) {
            // new UDP user
            i32 user_socket = socket(AF_INET, SOCK_DGRAM, 0);
            if (user_socket == -1) {
                eprintf("Failed to create socket: %s", strerror(errno));
                continue;
            }

            struct sockaddr_in local_addr = {.sin_family = AF_INET,
                                             .sin_addr.s_addr = INADDR_ANY,
                                             // pick any available port
                                             .sin_port = 0};

            if (bind(user_socket, (struct sockaddr *)&local_addr,
                     sizeof(local_addr))) {
                eprintf("Failed to bind socket: %s", strerror(errno));
                close(user_socket);
                continue;
            }

            if (!configure_socket(user_socket)) {
                close(user_socket);
                continue;
            }

            add_udp_user(user_socket);

        } else {
            // message from existing user
            handle_user_message(input_event.data.fd);
        }

        // delete users in END state
        delete_users_in_end_state();

        // purge deleted users from table of users
        purge_deleted_users();
    }
}
