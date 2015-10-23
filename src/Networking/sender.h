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
    int writeDataToServer(std::string data);


private:
    int portNumber;

    int sockfd, portno ;
    struct sockaddr_in serv_addr;
    struct hostent *server;


};
