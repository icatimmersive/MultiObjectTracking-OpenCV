#include "sender.h"
#include <signal.h>

Sender::Sender(const char *hostName, int portNumber) : portno(portNumber), sockfd(-1)
{
    // ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    // Get server address
    server = gethostbyname(hostName);
    if (server == NULL) {
        fprintf(stderr,"ERROR no such host: %s\n", hostName);
        exit(1);
    }

    // Set socket address struct
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr,
           server->h_addr,
           server->h_length);
    serv_addr.sin_port = htons(portno);

    // Attempt initial connection
    if(!reconnect()) {
        fprintf(stderr, "Failed to connect to server\n");
        exit(1);
    }
}

bool Sender::writeDataToServer(std::string data)
{
    if(sockfd == -1) {
        fprintf(stderr, "Trying to reconnect... ");
        if(reconnect()) {
            fprintf(stderr, "Successfully reconnected\n");
        } else {
            fprintf(stderr, "Failed\n");
            return false;
        }
    }

    int n = send(sockfd, data.c_str(), strlen(data.c_str()),0);
    if(n < 0) {
        perror("ERROR writing to socket");
        close(sockfd);
        sockfd = -1;
        return false;
    }

    return true;
}

bool Sender::reconnect() {
    // Close if currently open
    if(sockfd != -1) {
        close(sockfd);
    }

    // Open socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        return false;
    }

    // Connect socket
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        return false;
    }

    return true;
}
