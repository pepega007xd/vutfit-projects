#ifndef CONFIG_H
#define CONFIG_H

#include "imports.h"

typedef enum { Udp, Tcp } TransportType;

typedef struct {
        TransportType transport_type;
        struct sockaddr_in address; // including the server port
        u16 udp_confirm_timeout;    // in milliseconds, default 250
        u8 udp_max_retransmissions; // default 3
} Config;

/// parses command line arguments, prints help and exits if arguments are
/// invalid
Config parse_args(i32 argc, char **argv);

#endif // !CONFIG_H
