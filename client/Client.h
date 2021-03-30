//
// Created by andrey on 30.03.21.
//

#ifndef TCPEXCHANGE_CLIENT_H
#define TCPEXCHANGE_CLIENT_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>

class Client {

public:
    Client(int argc, const char **argv);

    enum Type
    {
        TCP,
        UDP
    };

private:
    // common
    Type type;
    struct sockaddr_in serv_addr{0};
    void help(const std::string &name);
    void write(int socket_fd, std::string prefix);
    void read(int socket_fd, std::string prefix);
    // tcp
    void tcpStream(int socket_fd);
    std::string tcp_prefix = "[TCP]";
    // udp
    void udpStream(int socket_fd);
    std::string udp_prefix = "[UDP]";

};


#endif //TCPEXCHANGE_CLIENT_H
