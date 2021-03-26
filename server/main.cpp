#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

using namespace std;

int main() {
    int listen_fd = 0;
    int conn_fd   = 0;
    char sendBuff[]{"Hello World TCP\n"};

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_addr{0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(4999);

    bind(listen_fd, (sockaddr*) &serv_addr, sizeof(serv_addr));
    listen(listen_fd, 10);

    cout << "Waiting for connection" << endl;
    conn_fd = accept(listen_fd, (sockaddr*) NULL, NULL);

    if(conn_fd < 0)
    {
        cout << "Connection is failed" << endl;
        return 1;
    }

    cout << "Connection is established" << endl;
    while (true) {

        ssize_t ok = send(conn_fd, sendBuff, strlen(sendBuff), MSG_NOSIGNAL);
        sleep(1);
        if (ok < 0) {
            close(conn_fd);
            cout << "Connection is closed from the client side" << endl;
            break;
        }
    }

    return 0;
}