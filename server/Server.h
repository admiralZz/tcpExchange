//
// Created by andrey on 29.03.21.
//

#ifndef TCPEXCHANGE_SERVER_H
#define TCPEXCHANGE_SERVER_H

#include <cstring>
#include <unistd.h>
#include <regex>
#include <thread>
#include <arpa/inet.h>
#include <iostream>

class Server {
public:
    static Server& getInstance()
    {
        static Server inst;
        return inst;
    }
    std::string get_numbers(std::string input);
    std::string get_ipaddr(int conn_fd);

private:
    //common
    Server();
    Server( const Server &inst) = delete;
    void operator=(const Server &inst) = delete;
    void remove_all(const std::string &rgx, std::string &input);

    //tcp
    void runTcpStream(const sockaddr_in &serv_addr);
    void tcpConnect(int conn_fd);
    void runUDPStream();

    int tcp_sock;
    int udp_sock;
};


#endif //TCPEXCHANGE_SERVER_H
