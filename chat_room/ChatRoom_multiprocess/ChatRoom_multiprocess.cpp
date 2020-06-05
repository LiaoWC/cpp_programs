/* Single process multi-process */
/* should compile with "-lrt" */
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
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <error.h>
// C++ Library
#include <atomic>
#include <exception>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>
// Custom Library
#include "cpp_headers/socket_programming.hpp"
#include "cpp_headers/string_manipulation.hpp"
#include "ChatRoomClient.hpp"
// define and namespace
#define SHM_NAME "ChatRoom_shm"
#define FTRUCATE_LEN 81920
#define ERRNO_BUF_SZ 256
using namespace std;

///////////////////////////////////////////// functions //////////////////////////////////////
typedef struct
{
        struct sockaddr_in addr_in;
        int sockfd;
        int UID;
} Client_struct;

typedef struct
{
        int onlineClientNum;
        int curLargestUID;
        Client_struct onlineClientList[MAX_ONLINE_CLIENTS];
        sem_t mutex;
} Client_Management;

Client_Management *clients;

void broadCast(Client_struct &sender, string msg)
{
        sem_wait(&(clients->mutex));

        // cout << "bc_UID: " << sender.UID << endl;
        // for (int i = 0; i < MAX_ONLINE_CLIENTS; i++)
        // {
        //         if (clients->onlineClientList[i].UID != 0)
        //         {
        //                 cout << "list[" << i << endl;
        //                 cout << clients->onlineClientList[i].addr_in.sin_addr.s_addr << endl;
        //                 cout << clients->onlineClientList[i].addr_in.sin_port << endl;
        //                 cout << clients->onlineClientList[i].addr_in.sin_family << endl;
        //                 cout << clients->onlineClientList[i].sockfd << endl;
        //                 cout << "] || UID: " << clients->onlineClientList[i].UID << endl;
        //         }
        // }

        msg = "\n>>> " + msg + "\n";

        for (int i = 0; i < MAX_ONLINE_CLIENTS; i++)
        {
                // cout << "i: " << i << endl;

                if (clients->onlineClientList[i].UID != 0)
                {
                        // cout << "hola UID: " << clients->onlineClientList[i].UID << endl;
                        cout << "sockfd: " << clients->onlineClientList[i].sockfd << endl;

                        if ((Client::sock_write(clients->onlineClientList[i].sockfd, msg)) < 0)
                        {
                                fprintf(stderr, "ERROR-write to UID: %d\n", clients->onlineClientList[i].UID);
                        }
                }
        }
        cout << "111" << endl;
        sem_post(&(clients->mutex));
}

void serveClient_multiprocess(Client_struct &client)
{
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        while (true)
        {
                // cout << "serveClient_curUID: " << client.UID << endl;
                // for (int i = 0; i < MAX_ONLINE_CLIENTS; i++)
                // {
                //         if (clients->onlineClientList[i].UID != 0)
                //                 cout << "list[" << i << "]: "
                //                      << "||| UID: " << clients->onlineClientList[i].UID << endl;
                // }

                Client::sock_write(client.sockfd, string("% "));
                memset(buffer, 0, sizeof(buffer));
                int recvBytes = recv(client.sockfd, buffer, BUFFER_SIZE, 0);
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
        // for (int i = 0; i < MAX_ONLINE_CLIENTS; i++)
        // {
        //         if (clients->onlineClientList[i].UID == client->UID)
        //         {
        //                 clients->onlineClientList[i] = nullptr;
        //                 break;
        //         }
        // }

        /* remove from the array */
        for (int i = 0; i < MAX_ONLINE_CLIENTS; i++)
        {
                if (clients->onlineClientList[i].UID == client.UID)
                {
                        clients->onlineClientList[i].UID = 0;
                        break;
                }
        }

        sem_wait(&(clients->mutex));

        /* close the socket */
        close(client.sockfd);

        /* free the memory */
        //free(client);
        --(clients->onlineClientNum);

        sem_post(&(clients->mutex));

        /* terminate the thread */
        exit(EXIT_SUCCESS);
}
int main(int argc, char *argv[])
{
        // char a[] = "26000";
        // argv[1] = a;

        /* check the arguments */
        if (argc != 2)
        {
                fprintf(stderr, "Invalid arguments. Usage: %s <port> \n", argv[0]);
                exit(EXIT_FAILURE);
        }

        /* declare and initi and others */
        char shmName[] = SHM_NAME;
        char errnoBuffer[ERRNO_BUF_SZ];
        char *errnoMsg = strerror_r(errno, errnoBuffer, ERRNO_BUF_SZ);
        int mSock, sSock;
        struct sockaddr_in cli_addr;
        socklen_t cli_size = sizeof(cli_addr);
        int shmfd = shm_open(shmName, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO); // O_RDWR is for read-write access // mode reference: https://www.itread01.com/content/1550098472.html
        if (ftruncate(shmfd, FTRUCATE_LEN) < 0)
        {
                // reference: https://stackoverflow.com/questions/7901117/how-do-i-use-errno-in-c
                fprintf(stderr, "ERROR-ftruncate: %s", errnoMsg);
        }
        clients = (Client_Management *)mmap(NULL, FTRUCATE_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
        sem_init(&(clients->mutex), 1, 1); // pshared > 0 is used for inter-process
        clients->curLargestUID = 0;
        clients->onlineClientNum = 0;
        for (int i = 0; i < MAX_ONLINE_CLIENTS; i++)
                clients->onlineClientList[i].UID = 0;

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
                if (clients->onlineClientNum >= MAX_ONLINE_CLIENTS)
                {
                        fprintf(stdout, "Refused the connection since it has reached the max online clients number.\n");
                        close(sSock);
                        continue;
                }

                /* record the new online client */
                //Client_struct *newClient = (Client_struct *)malloc(sizeof(Client_struct));

                /* insert into the list */
                int idx = 0;
                for (; idx < MAX_ONLINE_CLIENTS; idx++)
                {
                        if (clients->onlineClientList[idx].UID == 0)
                        {
                                // newClient->addr_in = cli_addr;
                                // newClient->sockfd = sSock;
                                // newClient->UID = clients->curLargestUID + 1;
                                // ++(clients->curLargestUID);
                                clients->onlineClientList[idx].UID = clients->curLargestUID + 1;
                                ++(clients->curLargestUID);
                                clients->onlineClientList[idx].sockfd = sSock;
                                clients->onlineClientList[idx].addr_in = cli_addr;
                                break;
                        }
                }

                /* create a new process */
                int fork_rt = fork();
                if (fork_rt > 0) // parent process
                        fprintf(stdout, "One new process created\n");
                else if (fork_rt == 0)
                        serveClient_multiprocess(clients->onlineClientList[idx]);
                else // < 0
                        fprintf(stderr, "ERROR-fork\n");
        }
}