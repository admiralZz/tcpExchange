#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <regex>
#include <thread>
#include <arpa/inet.h>
#include "Server.h"

using namespace std;

int main(int argc, const char **argv) {

    try {
        Server &s = Server::getInstance(argc, argv);
        s.run();
    }
    catch (int v)
    {
        cout << "Bad arguments" << endl;
        return -1;
    }
    catch (exception& e)
    {
        cout << "Failed run server" << endl;
        return -1;
    }

    return 0;
}