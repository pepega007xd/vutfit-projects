#ifndef STDIN_IO_H
#define STDIN_IO_H

#include "imports.h"
#include "user_input.h"

/// Receives message from STDIN, 'none' indicates that
/// the whole message has not yet been received,
/// or that it was handled internally (rename)
UserInputOption receive_user_input(void);

#endif // !STDIN_IO_H
