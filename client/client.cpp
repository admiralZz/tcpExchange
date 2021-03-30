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
#include "Client.h"

using namespace std;


int main(const int argc, const char **argv){
    
    Client client(argc, argv);
    
    return 0;
}
