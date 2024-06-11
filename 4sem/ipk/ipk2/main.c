#include "config.h"
#include "connection.h"
#include "event_loop.h"
#include "imports.h"

/// program configuration, set by parse_args, then read-only
Config config;

/// Connection is made global, because module `user` must have a way to
/// unregister user's socket from epoll interest list
Connection connection;

int main(int argc, char **argv) {
    // parse program arguments
    config = parse_args(argc, argv);

    // open authentication sockets, set up epoll
    connection = make_connection();

    // receive and process packets,
    // `run_event_loop` does not return
    run_event_loop();
}
