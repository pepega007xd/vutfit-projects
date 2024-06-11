#ifndef CONNECTION_H
#define CONNECTION_H

#include "imports.h"
#include "option.h"

typedef struct {
        i32 server_socket_fd;
} Connection;

/// establish connection to server (TCP mode),
/// or just create and bind server socket (UDP mode)
void make_connection(void);

/// close server socket
void close_connection(void);

#endif // !CONNECTION_H
