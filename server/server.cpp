#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <regex>
#include <thread>
#include <arpa/inet.h>
#include "Server.h"

using namespace std;

void remove_all(const string &rgx, string &input)
{
    regex r(rgx);
    smatch m;
    while(regex_search(input, m, r))
    {
        string s = m[0];
        input.erase(input.find(s), s.length());
    }
}

std::string get_numbers(std::string input)
{
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

std::string get_ipaddr(int conn_fd)
{
    struct sockaddr_in addr{};
    socklen_t  addr_size = sizeof(struct sockaddr_in);
    getpeername(conn_fd, (struct sockaddr*) &addr, &addr_size);

    return inet_ntoa(addr.sin_addr);
}

void connecting(int conn_fd)
{
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

int main() {

    Server& s = Server::getInstance();

    return 0;
}