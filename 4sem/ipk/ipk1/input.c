#include "input.h"
#include "config.h"
#include "connection.h"
#include "imports.h"
#include "state_machine_utils.h"
#include "stdin_io.h"
#include "tcp_io.h"
#include "udp_io.h"
#include "utils.h"

extern Config config;
extern Connection connection;
extern FSMTermination fsm_termination;

Input get_input(bool read_from_stdin) {
    while (true) {
        if (fsm_termination.kind != FSM_TERMINATION_NONE) {
            // return immediately, caller must handle termination
            return (Input){0};
        }

        i32 timeout = config.transport_type == Tcp ? -1 : get_timeout();

        struct pollfd poll_fds[2] = {0};
        poll_fds[0].fd = connection.server_socket_fd;
        poll_fds[0].events = POLLIN;
        poll_fds[1].fd = STDIN_FILENO;
        poll_fds[1].events = POLLIN;

        // second poll_fd (stdin) is used only when requested
        u8 inputs_count = read_from_stdin ? 2 : 1;
        i32 avaiable_inputs = poll(poll_fds, inputs_count, timeout);

        if (avaiable_inputs == -1 && errno == EINTR) {
            if (fsm_termination.kind == FSM_TERMINATION_BYE) {
                // if we got SIGINT, return immediately
                return (Input){0};
            } else {
                continue;
            }

        } else if (avaiable_inputs == -1) {
            print_exit("poll failed: %s", strerror(errno));

        } else if (avaiable_inputs == 0 && config.transport_type == Udp) {
            resend_messages();

        } else {
            Input input = {0};

            if ((poll_fds[0].revents & (POLLIN | POLLHUP)) != 0) {
                // handle message from server
                MessageOption message = config.transport_type == Tcp
                                            ? receive_tcp_message()
                                            : receive_udp_message();
                if (!message.is_valid) {
                    continue;
                }

                input.type = SERVER_MESSAGE;
                input.server_message = message.content;
            } else {
                // handle user input
                UserInputOption user_input = receive_user_input();
                if (!user_input.is_valid) {
                    continue;
                }
                input.type = USER_INPUT;
                input.user_input = user_input.content;
            }

            return input;
        }
    }
}
