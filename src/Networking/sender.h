#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>


#define SERVER_PORT_NUMBER 9999


class Sender {
public:
    Sender(const char *hostName, int portNumber);
    bool writeDataToServer(std::string data);


private:
    int portNumber;
    int sockfd;
    int portno;
    sockaddr_in serv_addr;
    hostent *server;

    bool reconnect();
};
