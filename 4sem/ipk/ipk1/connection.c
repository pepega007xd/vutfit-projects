#include "connection.h"
#include "config.h"
#include "imports.h"
#include "utils.h"

extern Config config;
extern Connection connection;

void set_as_nonblocking(i32 fd) {
    i32 flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        exit_fail();
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        exit_fail();
    }
}

void make_connection(void) {
    // create socket
    connection.server_socket_fd = socket(
        AF_INET, config.transport_type == Tcp ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (connection.server_socket_fd == -1) {
        print_exit("Could not create socket: %s", strerror(errno));
    }

    // bind socket
    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = 0; // bind to any available port

    if (bind(connection.server_socket_fd, (struct sockaddr *)&local_addr,
             sizeof(local_addr)) == -1) {
        eprintf("Could not bind socket: %s", strerror(errno));
        exit_fail();
    }

    // only connect to server if we're in tcp mode
    if (config.transport_type == Tcp) {
        if (connect(connection.server_socket_fd,
                    (struct sockaddr *)&config.address,
                    sizeof(config.address)) == -1) {
            eprintf("Could not connect to server: %s", strerror(errno));
            exit_fail();
        }
    }

    set_as_nonblocking(STDIN_FILENO);
    set_as_nonblocking(connection.server_socket_fd);
}

void close_connection(void) { close(connection.server_socket_fd); }
