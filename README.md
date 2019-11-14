![The LDPL Socket Library](images/ldpl-socket-logo.png)

The **LDPL Socket Library** allows you to open, close, write to, and read from network sockets in [**LDPL**](https://www.github.com/lartu/ldpl).

It's mostly made for clients - if you want to write a server, please see the library that this one is heavily based on: Lartu's https://www.github.com/lartu/ldpl-net-server

Requires **LDPL 4.3** or greater.

## Installation

Include the library into your LDPL project by copying the folder *ldpl_socket* to your project directory and then adding the line:

`INCLUDE "ldpl_socket/ldpl_socket.ldpl"`

before the `DATA` and `PROCEDURE` sections of your main LDPL project file. The library is now ready to be used.

## Usage

This library adds a few new statements to the language:

- `SOCKET CONNECT TO <hostname> AT <port> IN <number variable>`
   - Use this statement to open a new socket connection. Currently only TCP connections are supported. If the connection is successfully opened, your `<number var>` will be set to a "socket number" that's >= 0. Otherwise it's an error.
- `SOCKET CLOSE <socket number>`
   - Once a socket has been opened, use this statement to close it.
- `SOCKET SEND <text> TO <socket number>`
   - Use this to send messages to an open connection.
- `SOCKET READ FROM <socket number> IN <text variable>`
   - This statement should be used to check for messages on a socket connection opened with `SOCKET CONNECT TO <ip> PORT <port>`. `<text variable>` will be set to anything received from the socket, which may not be an entire "message" in whatever protocol you're using. 
   
## Example

There's an example echo client in `examples/echo-client.ldpl`. Use it with Lartu's demo echo server by cloning that project and running the template:

    git clone https://github.com/lartu/ldpl-net-server
    cd ldpl-net-server
    ldpl net_template.ldpl
    ./net_template-bin

Then, back in ldpl-socket land, build and run the client:

    make echo
    ./echo-client 

You should now be in a little echo repl:

    ~ Connected to localhost:8888 (socket:3)
    < Hello there!
    > Well, I never!
    < Well, I never!
    > ...or have I?
    < ...or have I?

## License

This library is released under the MIT License.
