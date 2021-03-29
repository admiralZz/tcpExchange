//
// Created by admiral on 29.03.21.
//

#include "Session.h"

using namespace std;

Session::Session(Server &server, int conn_fd): is_work(true)
{
    std::thread session(&Session::tcpConnect, this, std::ref(server), conn_fd);
    session.detach();
}

void Session::tcpConnect(Server& server, int conn_fd) {
    string prefix = "[THREAD CONN " + server.get_ipaddr(conn_fd) + "]:";
    cout << prefix << "Connection is established" << endl;
    char recvBuff[1024]{0};
    char sendBuff[1024]{0};

    while (is_work) {

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

        strcpy(sendBuff, server.get_numbers(input).c_str());
        ssize_t send_ok = send(conn_fd, sendBuff, strlen(sendBuff), 0);
        if(send_ok < 0)
            cout << prefix << "Error of sending answer" << endl;
        memset(recvBuff, 0, sizeof(recvBuff));
        memset(sendBuff, 0, sizeof(sendBuff));
    }
    close(conn_fd);
    cout << prefix << "Connection is closed from 'STOP' command" << endl;
}

void Session::stop() {
    is_work = false;
}
