#include "imports.h"
#include "state_machine_utils.h"
#include "utils.h"

extern FSMTermination fsm_termination;

void handler(int signo) {
    if (signo == SIGINT) {
        fsm_termination.kind = FSM_TERMINATION_BYE;
    }
}

void register_sigint_handler(void) {
    struct sigaction action = {0};
    action.sa_handler = handler;
    if (sigaction(SIGINT, &action, NULL) == -1) {
        print_exit("Registering signal handler failed: %s", strerror(errno));
    }
}
