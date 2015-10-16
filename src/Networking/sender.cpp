#include "sender.h"

Sender::Sender(const char *hostName, int portNumber)
{

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("ERROR opening socket");

    server = gethostbyname(hostName);
    portno = portNumber;


    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(0);
    }


}

int Sender::writeDataToServer(std::string data)
{
    int n;
    n = send(sockfd, data.c_str(), strlen(data.c_str()),0);
    //sleep(1);
    if (n < 0)
         perror("ERROR writing to socket");
    return 0;

}
