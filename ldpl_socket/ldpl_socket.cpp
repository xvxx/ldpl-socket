#include <stdio.h> 
#include <stdlib.h> 
#include "ldpl-types.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

extern ldpl_number VAR_ERRORCODE;
extern ldpl_text   VAR_ERRORTEXT;

#define BUF_SIZE 1024
#define DEBUG(text) { cout << "\e[33;1m" << text << "\e[0m" << endl; }
#define SETS_ERRORCODE() { VAR_ERRORCODE = 0; VAR_ERRORTEXT = ""; }

ldpl_text LDPL_SOCKET_MSG;
ldpl_text LDPL_SOCKET_IP;
ldpl_number LDPL_SOCKET_PORT;
ldpl_number LDPL_SOCKET_NUMBER; 
ldpl_number LDPL_SOCKET_BYTES; 

unordered_map<int, bool> blocking; // user-set socket state

void socket_connected(int socket_number, string ip, unsigned int port){
    LDPL_SOCKET_IP = ip;
    LDPL_SOCKET_PORT = port;
    LDPL_SOCKET_NUMBER = socket_number;
    blocking[socket_number] = true;
}

void socket_closed(int socket_number){
    LDPL_SOCKET_NUMBER = socket_number;
}

void socket_onmessage(int socket_number, string message){
    LDPL_SOCKET_NUMBER = socket_number;
    LDPL_SOCKET_MSG = message;
}

// set blocking vs non-blocking
void socket_set_flags(int sock){
    int flags = fcntl(sock, F_GETFL, 0);
    flags = blocking[sock] ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    fcntl(sock, F_SETFL, flags);
}

void LDPL_SOCKET_CONNECT(){
    SETS_ERRORCODE();
    const string host = LDPL_SOCKET_IP.str_rep();
    int port = LDPL_SOCKET_PORT;
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        VAR_ERRORCODE = sock;
        VAR_ERRORTEXT = "socket() failed";
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    struct hostent *h = gethostbyname(host.c_str());
    if(h == NULL){
        VAR_ERRORCODE = -1;
        VAR_ERRORTEXT = "bad hostname";
    }else if(h->h_length <= 0){
        VAR_ERRORCODE = -1;
        VAR_ERRORTEXT = "gethostbyname() failed";
    }
    char *ip = inet_ntoa(*(struct in_addr*)(h->h_addr_list[0]));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port        = htons(port);

    int err;
    if((err = connect(sock,(struct sockaddr*)&addr,sizeof(addr))) < 0){
        VAR_ERRORCODE = err;
        VAR_ERRORTEXT = "connect() failed";
    }
    if(sock >= 0) {
        socket_connected(sock, host, port);
        LDPL_SOCKET_NUMBER = sock;
    }
}

void LDPL_SOCKET_CLOSE(){
    int sock = LDPL_SOCKET_NUMBER;
    if(sock < 0) return;
    socket_closed(sock);
    close(sock); 
}

void LDPL_SOCKET_SENDMESSAGE(){
    SETS_ERRORCODE();
    LDPL_SOCKET_BYTES = 0;
    int sock = LDPL_SOCKET_NUMBER;
    socket_set_flags(sock);
    const string msg = LDPL_SOCKET_MSG.str_rep();

    int sent = 0, bytes = 0;
    while(sent < msg.size()){
        if((bytes = send(sock, msg.c_str(), msg.size(), 0)) < 0){
            VAR_ERRORCODE = bytes;
            VAR_ERRORTEXT = "send() call failed";
            return;
        }
        sent += bytes;
    }
    LDPL_SOCKET_BYTES = sent;
}

void LDPL_SOCKET_READ(){
    SETS_ERRORCODE();
    LDPL_SOCKET_BYTES = 0;
    int sock = LDPL_SOCKET_NUMBER;
    socket_set_flags(sock);

    char buf[BUF_SIZE];
    int bytes = read(sock, buf, BUF_SIZE);
    if(bytes < 0) {
        VAR_ERRORCODE = errno;
        VAR_ERRORTEXT = "socket read() error";
        LDPL_SOCKET_MSG = "";
        return;
    }else if(bytes == 0) {
        socket_closed(sock);
        VAR_ERRORCODE = 1;
        VAR_ERRORTEXT = "socket closed";
        LDPL_SOCKET_MSG = "";
        return;
    }
    buf[bytes] = 0;
    LDPL_SOCKET_MSG = buf;
    LDPL_SOCKET_BYTES = bytes;
}


void LDPL_SOCKET_SET_BLOCKING(){
    int sock = LDPL_SOCKET_NUMBER;
    blocking[sock] = true;
}

void LDPL_SOCKET_SET_NONBLOCKING(){
    int sock = LDPL_SOCKET_NUMBER;
    blocking[sock] = false;
}
