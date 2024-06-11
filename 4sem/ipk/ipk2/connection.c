#include "connection.h"
#include "config.h"
#include "utils.h"

extern Config config;

Connection make_connection(void) {
    Connection connection;

    // create sockets used for user authentication

    connection.tcp_auth_socket = socket(AF_INET, SOCK_STREAM, 0);
    connection.udp_auth_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (connection.tcp_auth_socket == -1 || connection.udp_auth_socket == -1) {
        print_exit("Failed to create socket: %s", strerror(errno));
    }

    // bind auth sockets

    int result1 =
        bind(connection.tcp_auth_socket, (struct sockaddr *)&config.address,
             sizeof(config.address));
    int result2 =
        bind(connection.udp_auth_socket, (struct sockaddr *)&config.address,
             sizeof(config.address));

    if (result1 == -1 || result2 == -1) {
        print_exit("Failed to bind socket: %s", strerror(errno));
    }

    // listen on tcp auth socket

    if (listen(connection.tcp_auth_socket, 1024) == -1) {
        print_exit("Failed to listen on socket: %s", strerror(errno));
    }

    // create epoll fd

    connection.epoll_fd = epoll_create1(0);

    if (connection.epoll_fd == -1) {
        print_exit("Failed to create epoll socket: %s", strerror(errno));
    }

    // add both auth sockets to epoll's interest list

    struct epoll_event event = {.data.fd = connection.tcp_auth_socket,
                                .events = EPOLLIN};

    result1 = epoll_ctl(connection.epoll_fd, EPOLL_CTL_ADD,
                        connection.tcp_auth_socket, &event);

    event.data.fd = connection.udp_auth_socket;
    result2 = epoll_ctl(connection.epoll_fd, EPOLL_CTL_ADD,
                        connection.udp_auth_socket, &event);

    if (result1 == -1 || result2 == -1) {
        print_exit("Failed to add auth socket to epoll: %s", strerror(errno));
    }

    return connection;
}
