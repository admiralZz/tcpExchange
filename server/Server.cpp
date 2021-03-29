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

Server::Server() {

    struct sockaddr_in serv_addr{0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(4999);

    initTcpStream(serv_addr);

}

void Server::initTcpStream(const sockaddr_in &serv_addr) {
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    bind(tcp_sock, (sockaddr*) &serv_addr, sizeof(serv_addr));
    listen(tcp_sock, 10);

    while (true) {
        cout << "Waiting for connection" << endl;
        int conn_fd = accept(tcp_sock, (sockaddr *) NULL, NULL);

        if (conn_fd < 0) {
            cout << "Connection is failed" << endl;
            continue;
        }

        std::thread session(&Server::tcpConnect, this, conn_fd);
        session.detach();
    }
}

void Server::initUDPStream() {

}

void Server::tcpConnect(int conn_fd) {
    string prefix = "[THREAD CONN " + get_ipaddr(conn_fd) + "]:";
    cout << prefix << "Connection is established" << endl;
    char recvBuff[1024]{0};
    char sendBuff[1024]{0};

    while (true) {

        ssize_t recv_ok = recv(conn_fd, recvBuff, sizeof(recvBuff), 0);
        if (recv_ok < 0) {
            close(conn_fd);
            cout << prefix << "Connection is closed" << endl;
            break;
        }
        if( recv_ok == 0 ) {
            close(conn_fd);
            cout << prefix << "Connection is closed from the client side" << endl;
            break;
        }

        string input(recvBuff);
        cout << prefix << input << endl;

        strcpy(sendBuff, get_numbers(input).c_str());
        ssize_t send_ok = send(conn_fd, sendBuff, strlen(sendBuff), 0);
        if(send_ok < 0)
            cout << prefix << "Error of sending answer" << endl;
        memset(recvBuff, 0, sizeof(recvBuff));
        memset(sendBuff, 0, sizeof(sendBuff));
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
    remove_all("-\\d+", input);
    remove_all("\\d+\\.\\d+", input);

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

    if(numbers.empty())
        return "";

    sort(numbers.begin(), numbers.end());

    string output;
    for( auto n : numbers)
        output += std::to_string(n) + " ";
    output += "\n" + std::to_string(sum);

    return output;
}

std::string Server::get_ipaddr(int conn_fd) {
    struct sockaddr_in addr{};
    socklen_t  addr_size = sizeof(struct sockaddr_in);
    getpeername(conn_fd, (struct sockaddr*) &addr, &addr_size);

    return inet_ntoa(addr.sin_addr);
}
