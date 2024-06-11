#include "config.h"
#include "imports.h"
#include "utils.h"

const char *help_message =
    "\nUsage: ipk24chat-server [-l <address>] [-p <port>] "
    "[-d <timeout>] [-r <retransmissions>] [-h]"
    "\n"
    "\nOptions:"
    "\n  -l <IP address>        IP address to listen on (default 0.0.0.0)"
    "\n  -p <port>              Server port to listen on (default: 4567)"
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

    char *address = "0.0.0.0";
    char *port = "4567";
    i32 opt;

    while ((opt = getopt(argc, argv, "l:p:d:r:h")) != -1) {
        switch (opt) {
        case 'l':;
            address = optarg;
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

    // we always get an IP address, no need to use `getaddrinfo`
    if (inet_pton(AF_INET, address, &config.address.sin_addr) != 1) {
        print_exit("Invalid IP address: %s", strerror(errno));
    }

    config.address.sin_family = AF_INET;
    config.address.sin_port = htons((u16)strtol(port, NULL, 10));

    return config;
}
