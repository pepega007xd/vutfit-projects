#ifndef CONFIG_H
#define CONFIG_H

#include "imports.h"

typedef struct {
        struct sockaddr_in address; // address + port to listen on (tcp and udp)
        u16 udp_confirm_timeout;    // in milliseconds, default 250
        u8 udp_max_retransmissions; // default is 3
} Config;

/// parses command line arguments into Config,
/// prints help and exits if there are invalid arguments
Config parse_args(i32 argc, char **argv);

#endif // !CONFIG_H
