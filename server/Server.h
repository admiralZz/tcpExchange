//
// Created by andrey on 29.03.21.
//

#ifndef TCPEXCHANGE_SERVER_H
#define TCPEXCHANGE_SERVER_H

#include <netinet/in.h>
#include <iostream>

class Server {
public:
    static Server& getInstance()
    {
        static Server inst;
        return inst;
    }
    class DataHandler
    {
        virtual std::string get_numbers(std::string input) = 0;
    };

private:
    //common
    Server();
    Server( const Server &inst) = delete;
    void operator=(const Server &inst) = delete;
    void remove_all(const std::string &rgx, std::string &input);
    std::string get_numbers(std::string input);
    std::string get_ipaddr(int conn_fd);

    //tcp
    void initTcpStream(const sockaddr_in &serv_addr);
    void tcpConnect(int conn_fd);
    void initUDPStream();

    int tcp_sock;
    int udp_sock;
};


#endif //TCPEXCHANGE_SERVER_H
