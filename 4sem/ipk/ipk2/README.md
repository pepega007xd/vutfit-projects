# IPK project 1

## Basic theory

### TCP protocol

Transmission control protocol [RFC793] is a connection-oriented protocol, which guarantees reliable data transfer in correct order. This is achieved by data segmentation at sending side, and reassembling at receiving side. Multiple connections from a single IP address are possible using different ports. Since the protocol allows for reliable data transfer, TCP connection is a byte stream - an application sends bytes, which are guaranteed to be received in that order. This means the data is not segmented in any way, and the underlying layer does not guarantee any particular segmentation during transfer. This means that data sent by a single `write` syscall might be received in multiple parts. When receiving a message over TCP, we must therefore find the end manually, and in case we receive only a part of a message, wait for the rest to arrive.

### UDP protocol 

User Datagram Protocol [RFC768] is a connectionless protocol, which transfers data in discrete packets (datagrams). The protocol does not guarantee delivery in any way, it is a responsibility of the application protocol to guarantee successful delivery if it is desired. UDP datagram payload is limited in length, but in our case, this is handled in the IPK24-CHAT protocol. When sending a message via UDP, the server must guarantee its delivery by awaiting a confirmation. This is also handled by the IPK24-CHAT protocol.

### Epoll API

The Linux kernel provides an API [epoll API] for waiting for avaliable data on multiple file descriptors. The API works by first allocating a file descriptor, adding multiple file descriptors into its *interest list*, and then calling `epoll_wait`. The function will block until some data is available to be read on any of the file descriptors from the interest list. This way, the application does not have to check in a loop whether any data is available on any of the file descriptors.

## Implementation

The application is implemented as a single-threaded application using an event loop which waits for the next message on `epoll_wait`. The specification does not describe a way to stop the server, so the only plausible way this server may stop is during an error. Even in this case, all file descriptors are closed, and all memory is deallocated.

This is the typical sequence of actions that happen during the server's runtime:

- The module `config` parses command line arguments, which are stored globally.
- The module `connection` opens two sockets (TCP and UDP) used for authentication. It also sets up epoll, and adds the authentication sockets to its interest list. These file descriptors are also stored globally.
- The event loop (module `event_loop`) starts, and `epoll_wait` waits for a message on the authentication sockets.
- An authentication message is received
    - in case the message is TCP, a socket for communication with the client is created automatically by `accept`
    - in case the message is UDP, a socket for communication with the client is created manually
- The client socket is configured to have a non-blocking read, and it is added to epoll's interest list.
- New user (module `user`) is created and added to a global table of users. The user is in AUTH state. All information related to this user is stored inside this `User` structure.
- The AUTH message for the user is received and processed by the user (function `handle_user_message_inner` in module `user`). This can fail only if a user with the same username is already logged in. This implementation allows each user to be logged in only once.
- The code returns to the main event loop and waits on `epoll_wait` for the next message.

When the server does not receive a confirmation of a UDP message in time, this sequence of actions happens:

- Before calling `epoll_wait`, the code calculates timeout as the time until the first unconfirmed UDP message needs to be retransmitted
- The call to `epoll_wait` times out
- Function `resend_messages` from module `user` goes through all UDP users, and for each one retransmits all unsent messages
    - if a message hits its maximum retransmit count, the connection is considered closed, and the user is deleted.

When receiving a message from an existing user (e.g. MSG), the following sequence of actions happens:
- `epoll_wait` returns a file descriptor not matching any of the authentication sockets.
- A user with matching file descriptor is found.
- The message is parsed, in case of UDP, confirmation is sent to the client
- user's FSM `handle_user_message_inner` is executed:
    - If the user is in OPEN state, message is sent to all users in the same channel (channel is stored simply as a string inside the `User` struct, there are no separate tables of channels, users per channel, etc.)
    - Otherwise, ERR and BYE messages are sent to the user, and user's state is set to END

Users in END state cannot be deleted immediately, UDP users must first have all their messages confirmed. Users are deleted once per event loop cycle, after handling received message (function `delete_users_in_end_state` in module `user`). This function deletes only users with no unconfirmed messages.

## Testing

I used the automatic tests written for the first project at [ipk1](https://git.fit.vutbr.cz/xbrabl04/ipk1) in file `test.py` to continuously validate the basic functionality of the server during development. For more specific usecases, I tested the server manually using the client from the first project, and `ncat` directly in the terminal.

The following is a list of tests done manually. Note that basic functionality, like sending messages between two clients, joining a channel was tested automatically using [test.py](https://git.fit.vutbr.cz/xbrabl04/ipk1/src/branch/master/test.py), so I did not repeat these tests manually.

Note that I am using [Nushell](https://www.nushell.sh/) as my system shell, which is not compatible with POSIX shell. When testing in e.g. bash, the exact commands would be different.

### Check how server responds to malformed TCP message

- Server setup: `./ipk24chat-server -l 127.0.0.1`
- Buggy client: `"AUTH user!name AS displayname USING 123\r\n" | nc 127.0.0.1 4567`
- Server output:
    ```
    ERR: received invalid message from user 
    SENT 127.0.0.1:35002 | ERR DisplayName=Server MessageContents=Received invalid message
    SENT 127.0.0.1:35002 | BYE
    ```
- Client output (raw from TCP socket):
    ```
    ERR FROM Server IS Received invalid message
    BYE
    ```
- Result: Server correctly sends an ERR message and terminates the connection


### Check how server detects lost connection in UDP

- UDP payload in file `udp-auth` (exported from Wireshark capture, see the [first project's documentation](https://git.fit.vutbr.cz/xbrabl04/ipk1/src/branch/master/README.md) for description of how I use Wireshark to capture testing traffic):
    ```
    0000   02 00 00 75 73 65 72 6e 61 6d 65 00 31 32 33 00   ...username.123.
    0010   64 6e 61 6d 65 00                                 dname.
    ```

- Server setup: `./ipk24chat-server -l 127.0.0.1`
- Buggy client: `cat udp-auth | nc -u 127.0.0.1 4567`
- Server output:
    ```
    RECV 127.0.0.1:48467 | AUTH Username=username DisplayName=123 Secret=dname
    SENT 127.0.0.1:48467 | CONFIRM
    SENT 127.0.0.1:48467 | REPLY Result=OK MessageContents=Login successful
    SENT 127.0.0.1:48467 | MSG DisplayName=Server MessageContents=123 has joined general.
    SENT 127.0.0.1:48467 | REPLY Result=OK MessageContents=Login successful
    SENT 127.0.0.1:48467 | MSG DisplayName=Server MessageContents=123 has joined general.
    SENT 127.0.0.1:48467 | REPLY Result=OK MessageContents=Login successful
    SENT 127.0.0.1:48467 | MSG DisplayName=Server MessageContents=123 has joined general.
    SENT 127.0.0.1:48467 | REPLY Result=OK MessageContents=Login successful
    SENT 127.0.0.1:48467 | MSG DisplayName=Server MessageContents=123 has joined general.
    ERR: Maximum UDP retransmit count reached for user username
    ```
- Result: Server tries to send REPLY and MSG messages to the client three times (default retry count), then fails and prints warning to `stderr`

### Check if the server can correctly reassemble a TCP message received in multiple chunks

- Server setup: `./ipk24chat-server -l 127.0.0.1`
- Buggy client: 
    ```
    mkfifo pipe-in # pipe is used so that I can manually send parts of a message, one at a time
    stdbuf -o0 cat pipe-in | stdbuf -o0 nc 127.0.0.1 4567 # stdbuf is used to disable input and output buffering

    # from another terminal, one after each other with delays
    "AUTH uname " | tee pipe-in
    "AS dname " | tee pipe-in
    "USING 123" | tee pipe-in
    "\r\n" | tee pipe-in
    ```
- Server output:
    ```
    RECV 127.0.0.1:46828 | AUTH Username=uname DisplayName=dname Secret=123
    SENT 127.0.0.1:46828 | REPLY Result=OK MessageContents=Login successful
    SENT 127.0.0.1:46828 | MSG DisplayName=Server MessageContents=dname has joined general.
    ERR: TCP user disconnected
    ERR: lost connection with user uname
    ```
- Client output:
    ```
    REPLY OK IS Login successful
    MSG FROM Server IS dname has joined general.
    ```
- Result: Server successfully reassembled the AUTH message. The client then forcibly closed the connection, and the server detected this too.

### Check that the server can correctly handle too long message

- Server setup: `./ipk24chat-server -l 127.0.0.1`
- Buggy client: `"auth uname as dname using 123\r\nmsg from dname is (python3 -c "print('a' * 1401)")\r\n" | nc 127.0.0.1 4567`
- Server output:
    ```
    RECV 127.0.0.1:51774 | AUTH Username=uname DisplayName=dname Secret=123
    SENT 127.0.0.1:51774 | REPLY Result=OK MessageContents=Login successful
    SENT 127.0.0.1:51774 | MSG DisplayName=Server MessageContents=dname has joined general.
    ERR: received invalid message from user uname
    SENT 127.0.0.1:51774 | ERR DisplayName=Server MessageContents=Received invalid message
    SENT 127.0.0.1:51774 | BYE
    ```
- Client output:
    ```
    REPLY OK IS Login successful
    MSG FROM Server IS dname has joined general.
    ERR FROM Server IS Received invalid message
    BYE
    ```
- Result: The server correctly rejects the message as invalid (maximum content length is 1400 characters)

## Bibliography

[RFC793] Postel, J. *Transmission Control Protocol* [online]. September 1981. [cited 2024-04-22]. DOI: 10.17487/RFC0793. Available at: [https://datatracker.ietf.org/doc/html/rfc793](https://datatracker.ietf.org/doc/html/rfc793)

[RFC768] Postel, J. *User Datagram Protocol* [online]. August 1980. [cited 2024-04-22]. DOI 10.17487/RFC0768. Available at: [https://datatracker.ietf.org/doc/html/rfc768](https://datatracker.ietf.org/doc/html/rfc768)

[epoll API] *epoll(7) Linux User's Manual* [online]. [cited 2024-04-22]. Available at: [https://manpages.org/epoll/7](https://manpages.org/epoll/7)
