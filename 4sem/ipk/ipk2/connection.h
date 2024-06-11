#ifndef CONNECTION_H
#define CONNECTION_H

#include "imports.h"

/// Structure that holds
typedef struct {
        i32 tcp_auth_socket; // socket fd for TCP authentication
        i32 udp_auth_socket; // socker fd for UDP authentication
        i32 epoll_fd;        // fd used by epoll
} Connection;

/// creates and binds authentication sockets, sets up epoll for polling sockets
Connection make_connection(void);

#endif // !CONNECTION_H
