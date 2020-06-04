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
#include <map>
// Custom Library
#include "cpp_headers/socket_programming.hpp"
#include "cpp_headers/string_manipulation.hpp"
#include "ChatRoomClient.hpp"
using namespace std;

///////////////////////////////////////////// global //////////////////////////////////////////
atomic<int> onlineClientNum = {0};
int curLargestUID = 0;
map<int, Client *> onlineClientList;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
///////////////////////////////////////////// functions //////////////////////////////////////

void broadCast(Client *sender, string msg)
{
        pthread_mutex_lock(&mutex);

        msg = "\n>>> " + msg + "\n";

        for (auto &it : onlineClientList)
        {
                if ((sender->sock_write(it.second->sockfd, msg)) < 0)
                {
                        fprintf(stderr, "ERROR-write to UID: %d\n", it.first);
                }
        }

        pthread_mutex_unlock(&mutex);
}

void *serveClient_multithreads(void *newClient)
{
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));
        Client *client = (Client *)newClient;

        while (true)
        {
                client->sock_write(client->sockfd, string("% "));
                memset(buffer, 0, sizeof(buffer));
                int recvBytes = recv(client->sockfd, buffer, BUFFER_SIZE, 0);
                if (recvBytes > 0)
                {
                        string msg(buffer);
                        string_manipulation::string_rm_allChars(msg, string("\n\r"));
                        cout << "msg: " << msg << endl;
                        if (msg == "")
                                continue;
                        if (msg == "exit")
                                break;
                        cout << "A client public broadcast: " << msg << endl;
                        broadCast(client, msg);
                }
                // connect shut
                else if (recvBytes == 0)
                        break;
                // <0 (-1: error)
                else
                {
                        fprintf(stderr, "ERROR-recv\n");
                        break;
                }
        }

        /* remove from the map */
        auto itClient = onlineClientList.find(client->UID);
        if (itClient != onlineClientList.end())
                onlineClientList.erase(itClient);
        else
                fprintf(stderr, "WARNING:cannot find the uid to erase from the map.\n");
        /* close the socket */
        close(client->sockfd);

        /* free the memory */
        delete client;
        --onlineClientNum;

        /* terminate the thread */
        pthread_detach(pthread_self());
        return (void *)client;
}

int main(int argc, char *argv[])
{
        /* check the arguments */
        if (argc != 2)
        {
                fprintf(stderr, "Invalid arguments. Usage: %s <port> \n", argv[0]);
                exit(EXIT_FAILURE);
        }

        /* declare and initi and others */
        int mSock, sSock;
        // init_onlineClientList();
        pthread_t thread_ID;
        struct sockaddr_in cli_addr;
        socklen_t cli_size = sizeof(cli_addr);
        signal(SIGPIPE, SIG_IGN);

        /* sock-bind */
        mSock = socket_programming::bind_socket_localhost(argv[1], "tcp");

        /* listen */
        if (listen(mSock, MAX_LISTENERS) < 0)
        {
                fprintf(stderr, "ERROR-listen\n");
                exit(EXIT_FAILURE);
        }

        while (true)
        {
                /* accept */
                sSock = accept(mSock, (struct sockaddr *)&cli_addr, &cli_size);
                if (sSock < 0)
                {
                        fprintf(stderr, "ERROR-accept\n");
                        exit(EXIT_FAILURE);
                }

                /* check if it has reached the max num of online clients */
                if (onlineClientNum >= MAX_ONLINE_CLIENTS)
                {
                        fprintf(stdout, "Refused the connection since it has reached the max online clients number.\n");
                        close(sSock);
                        continue;
                }

                /* record the new online client */
                Client *newClient = new Client(cli_addr, sSock, curLargestUID + 1);
                ++curLargestUID;
                onlineClientList.insert(make_pair(newClient->UID, newClient));

                /* create a new thread */
                pthread_create(&thread_ID, NULL, serveClient_multithreads, (void *)newClient);
        }
}