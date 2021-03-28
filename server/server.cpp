#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <regex>
#include <thread>

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

    vector<int> numbers;
    regex r("\\d+");
    smatch m;
    int sum = 0;

    while(regex_search(input, m, r))
    {
        int n = stoi(m[0]);
        sum += n;
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

void connecting(int conn_fd)
{
    string prefix = "[THREAD CONN " + std::to_string(conn_fd) + "]:";
    cout << prefix << "Connection is established" << endl;
    char recvBuff[1024]{0};

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
        }

        string input(recvBuff);
        cout << prefix << input << endl;
        char sendBuff[1024]{0};
        strcpy(sendBuff, get_numbers(input).c_str());
        ssize_t send_ok = send(conn_fd, sendBuff, strlen(sendBuff), 0);
        if(send_ok < 0)
            cout << prefix << "Error of sending answer" << endl;
        memset(recvBuff, 0, sizeof(recvBuff));
    }
}

int main() {

    int socket_fd = 0;
    int conn_fd   = 0;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        cout << "Creating the socket is failed" << endl;
        return 1;
    }

    sockaddr_in serv_addr{0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(4999);

    bind(socket_fd, (sockaddr*) &serv_addr, sizeof(serv_addr));
    listen(socket_fd, 10);

    while (true) {
        cout << "Waiting for connection" << endl;
        conn_fd = accept(socket_fd, (sockaddr *) NULL, NULL);

        if (conn_fd < 0) {
            cout << "Connection is failed" << endl;
            continue;
        }

        std::thread th(connecting, conn_fd);
        th.detach();
    }

    return 0;
}