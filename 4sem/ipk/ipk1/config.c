#include "config.h"
#include "imports.h"
#include "utils.h"

const char *help_message =
    "\nUsage: ipk24chat-client -t <tcp|udp> -s <address> [-p <port>]"
    "[-d <timeout>] [-r <retransmissions>] [-h]"
    "\n"
    "\nOptions:"
    "\n  -t <tcp|udp>           Transport protocol used for connection"
    "\n  -s <server>            Server IP address or hostname"
    "\n  -p <port>              Server port (default: 4567)"
    "\n  -d <timeout>           UDP confirmation timeout in milliseconds "
    "(default: 250)"
    "\n  -r <retransmissions>   Maximum number of UDP retransmissions "
    "(default: 3)"
    "\n  -h                     Print this help message and exit";

Config parse_args(i32 argc, char **argv) {
    Config config = {
        .udp_confirm_timeout = 250, // ms
        .udp_max_retransmissions = 3,
    };

    bool transport_type_present = false;
    bool address_present = false;
    char *address = NULL;
    char *port = "4567";
    i32 opt;

    // parse arguments using getopt

    while ((opt = getopt(argc, argv, "t:s:p:d:r:h")) != -1) {
        switch (opt) {
        case 't':
            if (string_eq("tcp", optarg)) {
                config.transport_type = Tcp;
            } else if (string_eq("udp", optarg)) {
                config.transport_type = Udp;
            } else {
                print_exit("Invalid transport type: %s", optarg);
            }

            transport_type_present = true;
            break;

        case 's':;
            address = optarg;
            address_present = true;
            break;

        case 'p':
            port = optarg;
            break;

        case 'd':
            config.udp_confirm_timeout = (u16)strtol(optarg, NULL, 10);
            break;

        case 'r':
            config.udp_max_retransmissions = (u8)strtol(optarg, NULL, 10);
            break;

        case 'h':
        default:
            puts(help_message);
            exit(EXIT_FAILURE);
        }
    }

    if (!(transport_type_present && address_present)) {
        print_exit("-t and -s are required arguments");
    }

    // convert text address/hostname into `struct sockaddr_in`
    // (getaddrinfo can convert both hostname and ip address)
    struct addrinfo *result = NULL;
    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype =
            (config.transport_type == Tcp ? SOCK_STREAM : SOCK_DGRAM)};

    i32 error = getaddrinfo(address, port, &hints, &result);

    if (error != 0 || result == NULL) {
        print_exit("Cannot resolve address %s port %s: %s", address, port,
                   gai_strerror(error));
    }

    config.address = *(struct sockaddr_in *)result->ai_addr;

    freeaddrinfo(result);

    return config;
}
