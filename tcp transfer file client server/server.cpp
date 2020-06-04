#include "socket_programming.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#define defaltFileName "waiting_for_naming"
#define listenQueueLength 20
#define bufSize 3000
using namespace std;

int main(int argc, char *argv[])
{
        /* check the arguments and assign */
        if (argc != 2)
        {
                exit(EXIT_FAILURE);
        }
        const char host[] = "0.0.0.0";
        const char *service = argv[1];

        /* prepare the socket */
        int master_sockfd = socket_programming::bind_socket(host, service, "tcp");

        /* wait for connection*/
        if (listen(master_sockfd, listenQueueLength) == -1)
        {
                cout << "Listen Error" << endl;
        }
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int slave_sockfd = accept(master_sockfd, (struct sockaddr *)&client_addr, &addr_size);

        /* read and output to the file */

        ofstream ofs(defaltFileName, ios::binary | ios::trunc);
        if (!ofs.good())
        {
                cout << "Accessing the file name unsuccessfully." << endl;
                exit(EXIT_FAILURE);
        }
        char buf[bufSize];
        memset(buf, 0, bufSize * sizeof(char));
        int readBytes = 0;
        int sumReadBytes = 0;
        while ((readBytes = read(slave_sockfd, buf, bufSize)) != 0)
        {
                if (readBytes == -1)
                {
                        cout << "ERROR-read from socket." << endl;
                        exit(EXIT_FAILURE);
                }
                ofs.write(buf, readBytes);
                sumReadBytes += readBytes;
                //cout << "read: " << readBytes << " bytes." << endl;
        }
        ofs.close();

        /* name the file */
        string fileName;
        cout << "Be aware of that if the file name already exists, the file will be replace!" << endl;
        cout << "Please type the file name(path): ";
        cin >> fileName;
        if (rename(defaltFileName, fileName.c_str()))
                cout << "Name the file successfully." << endl;
        else
                cout << "Unsuccessfully name the file." << endl;

        close(slave_sockfd);
        close(master_sockfd);
        printf("Receive %d bytes totally.\n", sumReadBytes);
        return 0;
}