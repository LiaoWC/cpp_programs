#ifndef _CHATROOMCLIENT_HPP_
#define _CHATROOMCLIENT_HPP_
/* Single process multi-threads */
// C Library
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
// C++ Library
#include <atomic>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
// Custom Library
#include "cpp_headers/socket_programming.hpp"
#include "cpp_headers/string_manipulation.hpp"
// define
#define MAX_ONLINE_CLIENTS 10
#define BUFFER_SIZE 4096
#define MAX_LISTENERS 10
// namespace using
using namespace std;

class Client
{
public:
        struct sockaddr_in addr_in;
        int sockfd;
        int UID;
        Client(struct sockaddr_in addr_in, int sockfd, int UID);
        static int sock_write(int recvSockfd, string msg);
};

Client::Client(struct sockaddr_in addr_in, int sockfd, int UID)
{
        this->addr_in = addr_in;
        this->sockfd = sockfd;
        this->UID = UID;
}

int Client::sock_write(int recvSockfd, string msg)
{
        char *ptr = new char[msg.size() + 1];
        strcpy(ptr, msg.c_str());
        int rt = socket_programming::socket_write(recvSockfd, ptr, msg.size() + 1);
        delete[] ptr;
        return rt;
}

#endif
