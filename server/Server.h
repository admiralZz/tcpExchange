//
// Created by andrey on 29.03.21.
//

#ifndef TCPEXCHANGE_SERVER_H
#define TCPEXCHANGE_SERVER_H

#include <netinet/in.h>
#include <iostream>

class Server {
public:
    void run();
    static Server& getInstance(int argc, const char **argv)
    {
        static Server inst(argc, argv);
        return inst;
    }

private:
    Server(int argc, const char **argv);
    Server( const Server &inst) = delete;
    void operator=(const Server &inst) = delete;

    void help(const std::string &name);
    // удаление подстроки(рег.выражение) из строки
    void remove_all(const std::string &rgx, std::string &input);
    // поиск чисел
    std::string get_numbers(std::string input);
    // получение ip-адреса
    std::string get_ipaddr(int conn_fd);
    // сетевые настройки сервера
    struct sockaddr_in tcp_serv_addr{0};
    struct sockaddr_in udp_serv_addr{0};

    // tcp
    int tcp_sock{0};
    std::string tcp_prefix = "[TCP]";
    void runTCPStream();
    void tcpConnect(int conn_fd);
    // udp
    int udp_sock{0};
    std::string udp_prefix = "[UDP]";
    void runUDPStream();
    void udpConnect(int conn_fd);


};


#endif //TCPEXCHANGE_SERVER_H
