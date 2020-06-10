#ifndef _SOCKET_PROGRAMMING_HPP_
#define _SOCKET_PROGRAMMING_HPP_

#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <string>
using namespace std;

class socket_programming
{
public:
        static int bindOrConnect_socket(const char *host, const char *service, const char *transportation_type, int mode);
        static int bind_socket(const char *host, const char *service, const char *transportation_type);
        static int connect_socket(const char *host, const char *service, const char *transportation_type);
        static int bind_socket_localhost(const char *service, const char *transportation_type);
        static int socket_write(int sockfd, char *cstr, int length);
        static int socket_write(int sockfd, char singleChar);
        static int socket_write(int sockfd, const char *cstr, int length);
        static int socket_write(int sockfd, string str);

}; // end of class: namespace socket_programming

/* mode 0 is to bind; 1 is to connect. */
int socket_programming::bindOrConnect_socket(const char *host, const char *service, const char *transportation_type, int mode)
{
        /* check for transportation_type */
        const char tcp[] = "tcp";
        const char udp[] = "udp";
        if (!(strcmp(transportation_type, tcp) == 0 || strcmp(transportation_type, udp) == 0))
        {
                printf("The transportation_type is not tcp or udp.\n");
                return -1;
        }

        /* set up hints */
        struct addrinfo hints;
        //memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;                                                              // Allow IPv4 or IPv6
        hints.ai_socktype = (strcmp(transportation_type, "tcp") == 0 ? SOCK_STREAM : SOCK_DGRAM); // choose based on the transportation type
        hints.ai_flags = AI_PASSIVE;                                                              // "The returned socket address will contain the "wildcard address"--https://www.man7.org/linux/man-pages/man3/gai_strerror.3.html
        hints.ai_protocol = 0;                                                                    // any protocol
        hints.ai_canonname = NULL;
        hints.ai_addr = NULL;
        hints.ai_next = NULL;

        /* getaddrinfo */
        struct addrinfo *result;
        int getaddrinfo_rt = getaddrinfo(NULL, service, &hints, &result); // The NULL means I use the local host address.
        if (getaddrinfo_rt != 0)
        {
                /* Not equaling to zaro means unsuccess.*/
                fprintf(stderr, "ERROR-getaddrinfo: %s\n", gai_strerror(getaddrinfo_rt));
                //exit(EXIT_FAILURE);
                return -1;
        }

        /* "result" is a linked-list of addrinfos. */
        /* Try each one until successfully binding */
        int sockfd;
        struct addrinfo *i = result;
        for (; i != NULL; i = i->ai_next)
        {
                sockfd = socket(i->ai_family, i->ai_socktype, i->ai_protocol);

                if (sockfd == -1)
                        continue; // error, try next

                if (mode == 0)
                {
                        if (bind(sockfd, i->ai_addr, i->ai_addrlen) == 0)
                                break; // bind successfully
                }
                else
                {
                        if (connect(sockfd, i->ai_addr, i->ai_addrlen) == 0)
                                break; // bind successfully
                }

                close(sockfd); // close those sockfd which bind unsuccessfully
        }
        if (i == NULL)
        {
                /* No addrinfo in  the list bind successfully  */
                if (mode == 0)
                        fprintf(stderr, "ERROR-bind unsuccessfully.\n");
                else
                        fprintf(stderr, "ERROR-connect unsuccessfully.\n");
                //exit(EXIT_FAILURE);
                return -1;
        }
        freeaddrinfo(result);
        return sockfd;
}

int socket_programming::bind_socket(const char *host, const char *service, const char *transportation_type)
{
        return socket_programming::bindOrConnect_socket(host, service, transportation_type, 0);
}

int socket_programming::connect_socket(const char *host, const char *service, const char *transportation_type)
{
        return socket_programming::bindOrConnect_socket(host, service, transportation_type, 1);
}

int socket_programming::bind_socket_localhost(const char *service, const char *transportation_type)
{
        return socket_programming::bind_socket("0.0.0.0", service, transportation_type);
}

/* return how many bytes written; -1 for error */
int socket_programming::socket_write(int sockfd, char *cstr, int length)
{
        int totalWritten = 0;

        /* write will return the bytes it sent */
        int bytes_left = length;
        int bytes_written = 0;
        char *curPosPtr = cstr;

        for (; bytes_left > 0; bytes_left -= bytes_written)
        {
                bytes_written = write(sockfd, curPosPtr, bytes_left);
                if (bytes_written <= 0)
                {
                        if (errno == EINTR) // interrupted system call
                                bytes_written = 0;
                        else
                        {
                                fprintf(stderr, "ERROR-write: %s\a\n", strerror(errno));
                                return -1;
                        }
                }
                bytes_left -= bytes_written;
                curPosPtr += bytes_written;
                totalWritten += bytes_written;
        }
        return totalWritten;
}

int socket_programming::socket_write(int sockfd, char singleChar)
{
        char *ptr = new char[2];
        ptr[0] = singleChar;
        ptr[1] = '\0';
        int rt = socket_programming::socket_write(sockfd, ptr, 1);
        delete ptr;
        return rt;
}

int socket_programming::socket_write(int sockfd, const char *cstr, int length)
{
        char *ptr = new char[length];
        strcpy(ptr, cstr);
        int rt = socket_programming::socket_write(sockfd, ptr, length);
        delete[] ptr;
        return rt;
}

int socket_write(int sockfd, string str)
{
        char *ptr = new char[str.size() + 1];
        strcpy(ptr, str.c_str());
        int rt = socket_programming::socket_write(sockfd, ptr, str.size() + 1);
        delete[] ptr;
        return rt;
}

/* ======= simple client sample =======

#include "socket_programming.hpp"

int main()
{
        const char host []="127.0.0.1";
        const char service[] = "25001";
        int sockfd = socket_programming::connect_socket(host,service,"tcp");
        char name[]="Liao\n";
        send(sockfd,name,strlen(name),0);

        close(sockfd);        
}

*/

/* ======= simple server sample =======

#include "socket_programming.hpp"
#include <iostream>
using namespace std;
#define listenQueueLength 20
int main(int argc, char *argv[])
{
        if (argc != 2)
        {
                exit(EXIT_FAILURE);
        }
        const char *service = argv[1];

        int master_sockfd = socket_programming::bind_socket(host, service, "tcp");
        if (listen(master_sockfd, listenQueueLength) == -1)
        {
                cout << "Listen Error" << endl;
        }
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int slave_sockfd = accept(master_sockfd, (struct sockaddr *)&client_addr, &addr_size);
        char buf[99999];
        memset(buf, 0, 99999);
        recv(slave_sockfd, buf, 99999, 0);
        printf("receive from client: %s", buf);
        return 0;
}

*/

#endif