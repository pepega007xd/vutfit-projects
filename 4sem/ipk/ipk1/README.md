# IPK project 1

## Project overview

Chat client is implemented in the GNU version of C11. The main engineering principle upheld during the implementation was
to choose simple and robust constructs, instead of focusing on speed or extensibility. The application is single-threaded to avoid 
complexity associated with locking shared resources. Besides initialization code, the application is driven by
a finite state machine roughly corresponding to the one in assignment. TCP and UDP modes are implemented by having two IO modules with
compatible interfaces, so that the common code is not duplicated.

## Program modules

- `sgc.h`: memory allocator which allows for global deallocation before exit (note that objects are still being maually allocated,
    this module serves mainly two purposes: to deallocate global structures which are valid for the entire runtime of the program, and to serve
    as a precaution, if a `free` call were missing in the normal code, the program will not leak memory after exit)
- `config.h`: command line argument parsing, result stored globally in `config`
- `connection.h`: establishes connection to the server, socket stored globally in `connection`
- `message.h`: `Message` data structure serves as the output of parsing both TCP and UDP messages (rest of the code can therefore work
    with both TCP and UDP messages)
- `tcp_io.h`: code for sending and receiving TCP messages, to handle the possibility of receiving a single message in two parts, data
    received from socket is buffered until the message ending (`\r\n`) is found
- `udp_io.h`: code that handles sending and receiving UDP messages, internally handles CONFIRM messages, so that code using this module
    can be generic over transport method. Internally stores messages not confirmed by the server, and exposes functions for retransmitting
    unconfirmed messages with expired retransmission timeout.
- `stdin_io.h`: handles parsing user input from `stdin`, parser also emits non-fatal errors to the user about command syntax
- `input.h`: this module exposes a function that returns the next available input (either from user via `stdin`, or from server via
    network socket). The code uses `poll` to wait until input is ready, and handles possible UDP message retransmissions using timeout
    on `poll`.
- `state_machine.h`: contains the finite state machine that drives the application. Only states `START`, `AUTH` and `OPEN` are handled by
    the state machine explicitly as states, when the program reaches the transition to `ERROR` or `END` state, the function 
    `handle_fsm_termination` is used to send the remaining messages and to exit the application. This is done to reduce unneccessary
    complexity of the state machine, since the state transitions after `ERROR` have only a single path which can be represented using
    regular code.
- `option.h`, `vec.h`: used to automatically generate optinal and vector types for given type names.

*Note that not all modules are mentioned, since they are not important for the overall structure of the code.*

## Error handling

Errors that do not allow any further actions by the application (e.g. socket for server communication is closed by the server) result
in the immediate termination of the program. Before that, error message is printed, and all resources are released (open socket,
memory allocations).

Errors that allow for graceful termination of connection are delegated to the main event loop (state machine code) using the `fsm_termination`
global variable, which selects one of a few possible termination mechanisms (sending `ERR` and `BYE`, sending only `BYE`, silent exit), 
which correspond to different transitions in the FSM.

## Testing

Since it is quite difficult to distinguish correct communication from incorrect using automatic tests (the fact that the output is correct
does not mean that the messages being sent to the server are all correct), I decided to do most of the testing manually.
The provided reference server is heavily rate-limited, so I implemented my own server for client testing, and for future submission,
the code is in the repository at [ipk2](https://git.fit.vutbr.cz/xbrabl04/ipk2.git). Because most of the testing was done manually, by
sending messages from multiple users from terminal, I do not have any list of test inputs or outputs. However, the testing
procedure looked like this:

1. start Wireshark, listen on the loopback device, filter by protocol (ipk24-chat)
2. start server: `./ipk24chat-server -l 127.0.0.1`
3. start clients: `./ipk24chat-client -t (tcp|udp) -s localhost`
4. enter valid and invalid commands to the client input, check packets captured by Wireshark for possible errors

I also used `Ncat` to simulate receiving invalid packets from the server, the procedure would be similar to the above, but 
instead of the server, I would simply copy the binary representations of correct packets from Wireshark into a file, modify specific bytes,
and send it to the client using `cat malformed_input | nc -ul 127.0.0.1 4567`.

For some very rudimentary automatic testing, just to see if a change broke a major part of the application, I wrote a Python script
to send data from clients to server, and to validate the responses. The script is implemented in the file `test.py`. To run the tests,
the binary `ipk24chat-server` must be present in the directory `ipk2` next to this repository. The tests work by piping data into
the standard input of running clients, and reading out their outputs. The outputs are then compared to expected values. The tests focus
on checking the functionality of the client on valid inputs: sending messages, commands `auth`, `join`, `rename`, and handling malformed
user input (invalid characters, too long input). The test script can be ran using `make test`.
