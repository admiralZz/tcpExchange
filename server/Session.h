//
// Created by admiral on 29.03.21.
//

#ifndef TCPEXCHANGE_SESSION_H
#define TCPEXCHANGE_SESSION_H

#include "Server.h"

class Session {

public:
    Session(Server &server, int conn_fd);
    void stop();

private:
    void tcpConnect(Server &server, int conn_fd);
    bool is_work;

};


#endif //TCPEXCHANGE_SESSION_H
