//
// Created by admiral on 26.03.21.
//
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>

using namespace std;

void do_write(int socket_fd)
{
    string prefix = "[WRITE THREAD]: ";
    char sendBuff[1024]{0};

    while (true) {
        cin.getline(sendBuff, sizeof(sendBuff), '\n');

        ssize_t send_ok = send(socket_fd, sendBuff, strlen(sendBuff), 0);
        if (send_ok < 0) {
            cout << prefix << "Error of sending to server" << endl;
            return;
        }
//        if(send_ok == 0)
//        {
//            cout << prefix << "Connection is closed from server side" << endl;
//            return;
//        }
        memset(sendBuff, 0, sizeof(sendBuff));
    }
}

void do_read(int socket_fd)
{
    string prefix = "[READ THREAD]: ";
    char recvBuff[1024]{0};

    while(true) {
        ssize_t recv_ok = recv(socket_fd, recvBuff, sizeof(recvBuff), 0);
        if (recv_ok < 0) {
            cout << prefix << "Error of response from server" << endl;
            return;
        }
        if( recv_ok == 0 ) {
            cout << prefix << "Connection is closed from the server side" << endl;
            return;
        }

        cout << endl << recvBuff << endl;
        memset(recvBuff, 0, sizeof(recvBuff));
    }
}

int main(const int argc, const char **argv){

    if(argc != 3)
    {
        cout << "\n Usage: " << argv[0] << " <ip address> <port> \n" << endl;
        return 1;
    }

    int socket_fd = 0;


    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_fd < 0)
    {
        cout << "Creating the socket is failed" << endl;
        return 1;
    }

    struct sockaddr_in serv_addr{0};

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(atoi(argv[2]));

    int inet_ok = inet_pton(serv_addr.sin_family, argv[1], &serv_addr.sin_addr);
    if(inet_ok <= 0){
        cout << "Error of inet address" << endl;
        return 1;
    }

    int conn_ok = connect(socket_fd, (sockaddr*) &serv_addr, sizeof(serv_addr));
    if(conn_ok < 0)
    {
        cout << "Connection is failed" << endl;
        return 1;
    }

    std::thread thr_write(do_write, socket_fd);
    std::thread thr_read(do_read, socket_fd);
    thr_write.join();
    thr_read.join();

    close(socket_fd);

    return 0;
}
