#ifndef _USER_HPP_
#define _USER_HPP_
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <errno.h>
#include <error.h>
#include <limits>

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/Bucket.h>

/* 自寫的hpp */
#include "socket_stream_write.hpp"
#include "input_processing.hpp"
#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif                                       /* INADDR_NONE */
#define host_address_ "localhost"            // 在引號內輸入要的host address
#define service_protocol_ "20000"            // 在引號內輸入要的service_
#define end_read_string "<end@csnctuedutw@>" // 務必和server端一樣
#define s3_region Aws::S3::Model::BucketLocationConstraint::us_east_1
#define bufferSize 2560
#define read_loop_max 1000
using namespace std;
using namespace input_processing;

class User
{
private:
        string serverHostAddress;
        string serverProtocol;
        string lastRead;
        string lastSend;
        string lastInput;
        string restCMD;
        string restOpCut;
        string lastOpHeadCut;
        int sockfd;

public:
        User();
        ~User();
        void fetch_address_and_protocol();
        void fetch_address_and_protocol(char *host, char *protocol);
        void connectSock();
        string readFromSocket();
        void sendToSocket(string str);
        //bool createBucket(string bucketName);
        //bool createPostObject(string objectName);
        /* input/output */
        void readClientInput();
        /* string processing */
        string nextOpHeadCut();
        string nextSprCut();
        int nextOpNum();
        bool if_aStrInTheOtherStr(string aStr, string bStr);
        /* get private member */
        string get_serverHostAddress() const;
        string get_serverProtocol() const;
        string get_lastRead() const;
        string get_lastSend() const;
        string get_lastInput() const;
        string get_restCMD() const;
        string get_restOpCut() const;
        string get_lastOpHeadCut() const;
        int get_sockfd() const;
        /* set private member */
        void set_serverHostAddress(string str);
        void set_serverProtocol(string str);
        void set_lastRead(string str);
        void set_lastSend(string str);
        void set_lastInput(string str);
        void set_restCMD(string str);
        void set_restOpCut(string str);
        void set_lastOpHeadCut(string str);
        void set_sockfd(int sockfd);
};

int errexit(const char *format, ...)
{
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        exit(1);
}

User::User()
{
}

User::~User()
{
        close(sockfd);
}

void User::fetch_address_and_protocol()
/* get_serverHostAddress and get_serverProtocol (從define得到) */
{
        serverHostAddress = host_address_;
        serverProtocol = service_protocol_;
}

void User::fetch_address_and_protocol(char *host, char *protocol)
/* get_serverHostAddress and get_serverProtocol (從argv得到) */
{
        serverHostAddress = string(host);
        serverProtocol = string(protocol);
}

void User::connectSock()
/*
 * Arguments:
 *      host      - name of host to which connection is desired
 *      service   - service associated with the desired port
 *      transport - name of transport protocol to use ("tcp" or "udp")
 */
{
        const char *host = serverHostAddress.c_str();
        const char *service = serverProtocol.c_str();
        const char transport[] = {"tcp"};
        struct hostent *phe;    /* pointer to host information entry	*/
        struct servent *pse;    /* pointer to service information entry	*/
        struct protoent *ppe;   /* pointer to protocol information entry*/
        struct sockaddr_in sin; /* an Internet endpoint address		*/
        int s, type;            /* socket descriptor and socket type	*/

        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;

        /* Map service name to port number */
        if (pse = getservbyname(service, transport))
                sin.sin_port = pse->s_port;
        else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
                errexit("can't get \"%s\" service entry\n", service);

        /* Map host name to IP address, allowing for dotted decimal */
        if (phe = gethostbyname(host))
                memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
        else if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
                errexit("can't get \"%s\" host entry\n", host);

        /* Map transport protocol name to protocol number */
        if ((ppe = getprotobyname(transport)) == 0)
                errexit("can't get \"%s\" protocol entry\n", transport);

        /* Use protocol to choose a socket type */
        if (strcmp(transport, "udp") == 0)
                type = SOCK_DGRAM;
        else
                type = SOCK_STREAM;

        /* Allocate a socket */
        s = socket(PF_INET, type, ppe->p_proto);
        if (s < 0)
                errexit("can't create socket: %s\n", strerror(errno));

        /* Connect the socket */
        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
                errexit("can't connect to %s.%s: %s\n", host, service,
                        strerror(errno));
        sockfd = s;
        return;
}

string User::readFromSocket()
{
        /* 我自定義每次read都要到收到結束字串才能停止read，不然網路慢的話可能會出問題 */
        string readUntilNow;
        std::size_t found_pos;
        int i = 0;
        for (; i < read_loop_max; i++)
        {

                char buffer[bufferSize];
                bzero(buffer, bufferSize);
                if (read(sockfd, buffer, bufferSize) < 0)
                {
                        fprintf(stderr, "Read Error: %s\a\n", strerror(errno));
                        return string("Read Error");
                }
                readUntilNow += string(buffer);
                found_pos = readUntilNow.find(string(end_read_string));
                if (found_pos != std::string::npos)
                {
                        int lengthOfEndReadString = (string(end_read_string)).size();
                        readUntilNow.erase(readUntilNow.begin() + found_pos, readUntilNow.begin() + found_pos + lengthOfEndReadString);
                        readUntilNow = input_processing::string_rm_front_behind_space(readUntilNow);
                        break;
                }
        }
        if (i == read_loop_max)
        {
                /* it may be forever loop */
                fprintf(stderr, "ERROR: cannot find the end string of the messeage.\a\n");
        }
        lastRead = readUntilNow;
        return lastRead;
}

void User::sendToSocket(string str)
{
        str += " ";
        str += end_read_string;
        char *cstr = new char[(unsigned)str.size() + 1];
        strcpy(cstr, str.c_str());
        // 網路寫函式不負責將全部資料寫完之後再返回。write回傳值回傳了幾個bytes
        int bytes_left = 0;
        int bytes_written = 0;
        char *ptr = cstr;
        for (bytes_left = strlen(cstr); bytes_left > 0; bytes_left -= bytes_written)
        {
                bytes_written = write(sockfd, ptr, bytes_left);
                if (bytes_written <= 0) // 如果資料write有問題
                {
                        if (errno == EINTR) // 中斷錯誤
                                bytes_written = 0;
                        else
                        {
                                fprintf(stderr, "write ERROR: %s\a\n", strerror(errno));
                                exit(1);
                        }
                }
                bytes_left -= bytes_written;
                ptr += bytes_written;
        }
        lastSend = str;
        return;
}

// bool User::createBucket(string bucketName)
// {
//         // Set these values before compiling and running the program
//         bool ifSuccess = false;
//         const Aws::String bucket_name = (Aws::String)(bucketName.c_str());
//         //const Aws::S3::Model::BucketLocationConstraint region = s3_region;
//         //const Aws::S3::Model::BucketLocationConstraint region = Aws::S3::Model::BucketLocationConstraint::us_east_1

//         // Set up the request
//         Aws::S3::Model::CreateBucketRequest request;
//         request.SetBucket(bucket_name);

//         // Specify the region as a location constraint
//         //Aws::S3::Model::CreateBucketConfiguration bucket_config;
//         //bucket_config.SetLocationConstraint(region);
//         //request.SetCreateBucketConfiguration(bucket_config);

//         // Create the bucket
//         Aws::S3::S3Client s3_client;
//         auto outcome = s3_client.CreateBucket(request);
//         if (!outcome.IsSuccess())
//         {
//                 auto err = outcome.GetError();
//                 std::cout << "ERROR: CreateBucket: " << err.GetExceptionName() << ": " << err.GetMessage() << std::endl;
//                 ifSuccess = false;
//         }
//         else
//                 return true;
//         return ifSuccess;
// }

////////////////////////// input/output ////////////////////////
void User::readClientInput()
/* 印出"% "；讓client輸入指令 */
{
        printf("%% ");
        char inputBuffer[bufferSize];
        cin.getline(inputBuffer, bufferSize);
        lastInput = ((string)inputBuffer);
        return;
}

////////////////////////// string processing ////////////////////////
string User::nextOpHeadCut()
/* take the first piece as lastOpHeadCut from the restOpCut */
{
        vector<string> temp = string_cut_in_half(restOpCut);
        lastOpHeadCut = string_rm_front_behind_space(temp[0]);
        restOpCut = string_rm_front_behind_space(temp[1]);
        return lastOpHeadCut;
}

string User::nextSprCut()
/* split string by "<spr>", and put into restOpCut, and renew restCMD  */
{
        vector<string> inputVec;
        restCMD = string_rm_front_behind_space(restCMD);
        string keyWord("<spr>");
        inputVec = string_cut_by_word(restCMD, keyWord);
        if ((unsigned)inputVec.size() == 0)
        {
                cout << "nextSprCut failed since string_cut_by_word cannot find the key word." << endl;
                string rt("");
                return rt;
        }
        else
        {
                restCMD = string_rm_front_behind_space(inputVec[1]);
                restOpCut = string_rm_front_behind_space(inputVec[0]);
                return restOpCut;
        }
}

int User::nextOpNum()
{
        string numOp_str = nextSprCut();
        string::size_type sz;
        int numOp = 0;
        try
        {
                numOp = stoi(numOp_str, &sz);
        }
        catch (const std::exception &e)
        {
                cout << "Error when doing User::nextOpNum's stoi" << endl;
                std::cerr << e.what() << '\n';
                return -1;
        }
        return numOp;
}

////////////////////////// get private member ////////////////////////
string User::get_serverHostAddress() const
{
        return serverHostAddress;
}
string User::get_serverProtocol() const { return serverProtocol; }
string User::get_lastRead() const { return lastRead; }
string User::get_lastSend() const { return lastSend; }
string User::get_lastInput() const { return lastInput; }
string User::get_restCMD() const { return restCMD; }
string User::get_restOpCut() const { return restOpCut; }
string User::get_lastOpHeadCut() const { return lastOpHeadCut; }
int User::get_sockfd() const { return sockfd; }
////////////////////////// set private member ////////////////////////
void User::set_serverHostAddress(string str)
{
        serverHostAddress = str;
        return;
}
void User::set_serverProtocol(string str)
{
        serverProtocol = str;
        return;
}
void User::set_lastRead(string str)
{
        lastRead = str;
        return;
}
void User::set_lastSend(string str)
{
        lastSend = str;
        return;
}
void User::set_lastInput(string str)
{
        lastInput = str;
        return;
}
void User::set_restCMD(string str)
{
        restCMD = str;
        return;
}
void User::set_restOpCut(string str)
{
        restOpCut = str;
        return;
}
void User::set_lastOpHeadCut(string str)
{
        lastOpHeadCut = str;
        return;
}
void User::set_sockfd(int sockfd)
{
        this->sockfd = sockfd;
};
//////////////////////////  ////////////////////////
#endif