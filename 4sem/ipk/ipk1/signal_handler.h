#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

/// registers a function to handle the SIGINT signal (ctrl+c),
/// to not abort the program, but set a stop condition into the variable
/// `fsm_termination` (causes graceful exit within the event loop)
void register_sigint_handler(void);

#endif // !SIGNAL_HANDLER_H
