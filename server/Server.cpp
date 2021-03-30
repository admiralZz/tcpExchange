//
// Created by andrey on 29.03.21.
//

#include <cstring>
#include <unistd.h>
#include <regex>
#include <thread>
#include <arpa/inet.h>
#include "Server.h"

using namespace std;

Server::Server(int argc, const char **argv) {

    if(argc != 3)
    {
        help(argv[0]);
        throw -1;
    }
    try {
        tcp_serv_addr.sin_family = AF_INET;
        tcp_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        tcp_serv_addr.sin_port = htons(atoi(argv[1]));

        udp_serv_addr.sin_family = AF_INET;
        udp_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        udp_serv_addr.sin_port = htons(atoi(argv[2]));
    }
    catch (exception& e)
    {
        cout << "Init server is failed" << endl;
        throw e;
    }
}

void Server::runTCPStream() {
    // Создаем и настраиваем сокет
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    int bind_ok = bind(tcp_sock, (sockaddr*) &tcp_serv_addr, sizeof(tcp_serv_addr));
    if(bind_ok < 0)
    {
        cout << this->tcp_prefix << ":Binding is failed" << endl;
        return;
    }
    // Слушаем
    listen(tcp_sock, 10);

    while (true) {
        cout << this->tcp_prefix << ":Waiting for connection" << endl;
        // Получаем сокет сессии
        int conn_fd = accept(tcp_sock, (sockaddr *) NULL, NULL);

        if (conn_fd < 0) {
            cout << this->tcp_prefix << ":Connection is failed" << endl;
            continue;
        }
        // Запускаем сессию
        std::thread session(&Server::tcpConnect, this, conn_fd);
        session.detach();
    }
}

void Server::runUDPStream() {
    // Открываем udp сокет
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    int bind_ok = bind(udp_sock, (sockaddr*) &udp_serv_addr, sizeof(udp_serv_addr));
    if(bind_ok < 0)
    {
        cout << this->udp_prefix << ":Binding is failed" << endl;
        return;
    }
    cout <<  this->udp_prefix << ":Waiting for connection" << endl;

    udpConnect(udp_sock);
}

void Server::tcpConnect(int conn_fd) {
    string prefix = this->tcp_prefix + "[SESSION " + get_ipaddr(conn_fd) + "]:";
    cout << prefix << "Connection is established" << endl;
    char recvBuff[1024];
    char sendBuff[1024];

    while (true) {

        memset(recvBuff, 0, sizeof(recvBuff));
        memset(sendBuff, 0, sizeof(sendBuff));

        ssize_t recv_ok = recv(conn_fd, recvBuff, sizeof(recvBuff), 0);
        if (recv_ok < 0) {
            cout << prefix << "Connection is closed" << endl;
            break;
        }
        if( recv_ok == 0 ) {
            cout << prefix << "Connection is closed from the client side" << endl;
            break;
        }

        string input(recvBuff);
        cout << prefix << input << endl;

        string answer = get_numbers(input);
        if(!answer.empty()) {
            strcpy(sendBuff, get_numbers(input).c_str());
            ssize_t send_ok = send(conn_fd, sendBuff, strlen(sendBuff), 0);
            if (send_ok < 0)
                cout << prefix << "Error of sending answer" << endl;
        }
    }
    close(conn_fd);
}

void Server::udpConnect(int conn_fd) {

    char recvBuff[1024];
    char sendBuff[1024];

    while (true)
    {
        memset(recvBuff, 0, sizeof(recvBuff));
        memset(sendBuff, 0, sizeof(sendBuff));

        struct sockaddr_in client_addr;
        int client_addr_size = sizeof(client_addr);
        int recv_ok = recvfrom(conn_fd, &recvBuff, sizeof(recvBuff), 0, (struct sockaddr*) &client_addr, (socklen_t *) &client_addr_size);

        if(recv_ok < 0)
        {
            cout << this->udp_prefix << "Error of recv data" << endl;
            continue;
        }
        string addr_name = inet_ntoa(client_addr.sin_addr);
        string prefix = this->udp_prefix + "[UDP CONN " + addr_name + "]: ";
        string input(recvBuff);
        cout << prefix << input << endl;

        string answer = get_numbers(input);
        if(!answer.empty()) {
            strcpy(sendBuff, get_numbers(input).c_str());
            ssize_t send_ok = sendto(conn_fd, sendBuff, strlen(sendBuff), 0, (const sockaddr *) &client_addr,
                                     client_addr_size);
            if (send_ok < 0) {
                cout << prefix << "Error of send data" << endl;
            }
        }
    }
}

void Server::remove_all(const std::string &rgx, std::string &input) {
    regex r(rgx);
    smatch m;
    while(regex_search(input, m, r))
    {
        string s = m[0];
        input.erase(input.find(s), s.length());
    }
}

std::string Server::get_numbers(std::string input) {
    // Удаляем отрицательные и дробные числа
    remove_all("-\\d+", input);
    remove_all("\\d+\\.\\d+", input);
    // Ищем числа
    vector<unsigned long long> numbers;
    regex r("\\d+");
    smatch m;
    unsigned long long sum = 0;

    while(regex_search(input, m, r))
    {
        unsigned long long n;
        try {
            n = stoull(m[0]);
            sum += n;
        }
        catch (std::exception e)
        {
            return "Input data is not correct";
        }

        numbers.push_back(n);
        input = m.suffix();
    }
    // Если чисел не нашли
    if(numbers.empty())
        return "";

    // Формируем ответную строку
    sort(numbers.begin(), numbers.end());
    string output = "numbers: ";
    for( auto n : numbers)
        output += std::to_string(n) + " ";
    output += "\nsum: " + std::to_string(sum);

    return output;
}

std::string Server::get_ipaddr(int conn_fd) {
    struct sockaddr_in addr{};
    socklen_t  addr_size = sizeof(struct sockaddr_in);
    getpeername(conn_fd, (struct sockaddr*) &addr, &addr_size);

    return inet_ntoa(addr.sin_addr);
}

void Server::run() {
    std::thread th_tcp(&Server::runTCPStream, this);
    std::thread th_udp(&Server::runUDPStream, this);

    th_tcp.join();
    th_udp.join();

    close(tcp_sock);
    close(udp_sock);
}

void Server::help(const std::string &name) {
    cout << "Usage: " << name << " <TCP port> <UDP port>" << endl;
}


