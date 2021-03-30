//
// Created by andrey on 30.03.21.
//

#include "Client.h"

using namespace std;

Client::Client(int argc, const char **argv) {
    if(argc != 4){
        help(argv[0]);
        return;
    }

    // Определяем тип соединения
    int socket_fd = 0;
    string type = argv[1];
    if(type == "TCP") {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        this->type = Type::TCP;
    }
    else if(type == "UDP") {
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        this->type = Type::UDP;
    }
    else
    {
        help(argv[0]);
        return;
    }

    if(socket_fd <= 0)
    {
        cout << "Creating the socket is failed" << endl;
        return;
    }

    // Определяем сетевые настройки
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(atoi(argv[3]));
    int inet_ok = inet_pton(serv_addr.sin_family, argv[2], &serv_addr.sin_addr);
    if(inet_ok <= 0){
        cout << "Error of inet address" << endl;
        return;
    }

    if(this->type == Type::TCP)
    {
        tcpStream(socket_fd);
    }
    else if(this->type == Type::UDP)
    {
        udpStream(socket_fd);
    }

}

void Client::tcpStream(int socket_fd) {
    // Устанавливаем соединение с сервером
    int conn_ok = connect(socket_fd, (sockaddr*) &serv_addr, sizeof(serv_addr));
    if(conn_ok < 0)
    {
        cout << this->tcp_prefix << "Connection is failed" << endl;
        return;
    }
    else
        cout << this->tcp_prefix << "Connection is established" << endl;

    // Запускаем потоки чтения/записи
    std::thread thr_write(&Client::write, this, socket_fd, this->tcp_prefix);
    std::thread thr_read(&Client::read, this, socket_fd, this->tcp_prefix);
    thr_write.join();
    thr_read.join();

    // Закрываем соединение
    close(socket_fd);
}

void Client::udpStream(int socket_fd) {
    // Запускаем потоки чтения/записи
    cout << this->udp_prefix << "Connection is established" << endl;
    std::thread thr_write(&Client::write, this, socket_fd, this->udp_prefix);
    std::thread thr_read(&Client::read, this, socket_fd, this->udp_prefix);
    thr_write.join();
    thr_read.join();

    // Закрываем соединение
    close(socket_fd);
}

void Client::write(int socket_fd, string prefix)
{
    prefix += "[enter]: ";
    char sendBuff[1024];

    while (true) {
        memset(sendBuff, 0, sizeof(sendBuff));
        
        // Ждем ввода сообщения и обрезаем строку если она больше 1024
        cout << prefix;
        string input;
        getline(cin, input);
        // Не отправляем пустую строку
        if(input.empty())
            continue;
        strncpy(sendBuff, input.c_str(), input.length());
        sendBuff[1023] = '\0';

        // Пишем в сокет
        ssize_t send_ok;
        if(type == Type::TCP)
            send_ok = send(socket_fd, sendBuff, strlen(sendBuff), 0);
        else if(type == Type::UDP)
            send_ok = sendto(socket_fd, sendBuff, strlen(sendBuff), 0, (const struct sockaddr*) &serv_addr, sizeof(serv_addr));
        else
        {
            cout << prefix << "Undefined connection type" << endl;
            break;
        }

        if (send_ok < 0) {
            cout << prefix << "Error of sending to server" << endl;
            break;
        }
        // Задержка для получения ответа
        usleep(500000);
    }
}

void Client::read(int socket_fd, string prefix)
{
    prefix += "[answer]: ";
    char recvBuff[1024];

    while(true) {
        memset(recvBuff, 0, sizeof(recvBuff));

        // Читаем сокет
        ssize_t recv_ok;
        if(type == Type::TCP)
            recv_ok = recv(socket_fd, recvBuff, sizeof(recvBuff), 0);
        else if(type == Type::UDP) {
            socklen_t len = sizeof(serv_addr);
            recv_ok = recvfrom(socket_fd, recvBuff, sizeof(recvBuff), 0, (struct sockaddr *) &serv_addr, &len);
        }
        else
        {
            cout << prefix << "Undefined connection type" << endl;
            break;
        }

        if (recv_ok <= 0) {
            cout << prefix << "Error of response from server" << endl;
            break;
        }
        // Выводим ответ
        cout << endl << prefix << recvBuff << endl;
    }
}

void Client::help(const std::string &name) {
    cout << "\n Usage: " << name << " <type(UDP, TCP)> <ip address> <port> \n" << endl;
}

