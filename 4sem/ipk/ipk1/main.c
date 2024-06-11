#include "config.h"
#include "connection.h"
#include "imports.h"
#include "signal_handler.h"
#include "state_machine.h"

// command line configuration of program, changed only here, and when REPLY
// message is delivered from a new server port on UDP mode
Config config;

// socket for connection to the server, changed only here and when closing the
// socket during exit
Connection connection;

i32 main(i32 argc, char **argv) {
    // parse arguments and translate hostname to ip address
    config = parse_args(argc, argv);

    // establish connection to server, set up epoll fds
    // (modifies global variable `connection`)
    make_connection();

    // register signal handler function to gracefully
    // terminate connection instead of aborting
    register_sigint_handler();

    // run the main FSM event loop
    // (does not return, handles exit internally)
    run_state_machine();
}
