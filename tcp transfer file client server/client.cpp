#include "socket_programming.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#define bufSize 100000
using namespace std;

int main(int argc, char *argv[])
{
        /* check the arguments and assign */
        if (argc != 3)
        {
                std::cout << "Too few arguments. (Server-ip and Server-port)" << std::endl;
                exit(EXIT_FAILURE);
        }
        const char *host = argv[1];
        const char *service = argv[2];

        /* connect to the server */
        int sockfd = socket_programming::connect_socket(host, service, "tcp");
        if (sockfd < 0)
        {
                cout << "Socket Connecting ERROR" << endl;
                exit(EXIT_FAILURE);
        }

        /* deal with the file the user want to send */
        string fileName;
        cout << "Please type the file name: ";
        cin >> fileName;
        ifstream ifs(fileName.c_str(), ifstream::binary);
        if (!ifs.good())
        {
                cout << "Accessing the file unsuccessfully." << endl;
                exit(EXIT_FAILURE);
        }

        struct stat stat_buffer;
        if (stat(fileName.c_str(), &stat_buffer) != 0)
        {
                cout << "ERROR-get stat of the file" << endl;
        }
        long long int fileSize = stat_buffer.st_size;
        cout << "The file size is " << fileSize << "." << endl;

        /* send */
        long long int loop = (fileSize % bufSize == 0) ? fileSize / bufSize : fileSize / bufSize + 1;
        int lastSendSize = fileSize % bufSize;
        char singleCharBuf;
        char buf[bufSize];
        long long int totalWritten = 0;
        //while (ifs.get(singleCharBuf))
        //cout << "loop: " << loop << endl;
        while (true)
        {
                if (loop == 0)
                        break;
                int sizeOneTime = (loop == 1) ? lastSendSize : bufSize;
                ifs.read(buf, sizeOneTime);
                //cout << "cur loop: " << loop << " | sizeOneTime: " << sizeOneTime << endl;
                totalWritten += socket_programming::tcp_write(sockfd, buf, sizeOneTime);
                loop--;
        }

        // /* send one char every time */
        // char singleCharBuf;
        // long long int totalWritten = 0;
        // while (ifs.get(singleCharBuf))
        //         totalWritten += socket_programming::tcp_write(sockfd, singleCharBuf);

        /* end */
        ifs.close();
        close(sockfd);
        cout << "Write " << totalWritten << " bytes totally." << endl;
        return 0;
}