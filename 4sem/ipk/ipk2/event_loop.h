#ifndef EVENT_LOOP
#define EVENT_LOOP

#include "imports.h"

/// Waits using epoll until a packet can be received, then processes the packet.
/// This is done in a loop.
/// UDP retransmissions are handled using timeout on epoll_wait.
noreturn void run_event_loop(void);

#endif // !EVENT_LOOP
