#ifndef _socket_stream_write_hpp
#define _socket_stream_write_hpp

#include "headers.hpp"

namespace socket_stream_write
{

///////////////////////////
// 將cstring寫入sockfd
///////////////////////////
//
// 表示不用顯示資訊在server端
// 表示 ip , clinet no.
void socket_stream_write(int sockfd, char string[], char ip_dot_decimal[INET_ADDRSTRLEN], int clientNum)
{
        /* 網路寫函式不負責將全部資料寫完之後再返回。write回傳值回傳了幾個bytes */
        int bytes_left = 0;
        int bytes_written = 0;
        char *ptr = string;

        fprintf(stdout, "### write to ClientNO.%d(IP %s):\n", clientNum, ip_dot_decimal);

        for (bytes_left = strlen(string); bytes_left > 0; bytes_left -= bytes_written)
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
        // 成功傳完
        fprintf(stdout, "%s\n", string);
        return;
}

/////////////////////////
// 要write的字串傳入的是std::string
/////////////////////////
void socket_stream_write(int sockfd, std::string s, char ip_dot_decimal[INET_ADDRSTRLEN], int clientNum)
{
        char *cstr = new char[s.length()];
        strcpy(cstr, s.c_str());
        socket_stream_write(sockfd, cstr, ip_dot_decimal, clientNum);
        delete[] cstr;
        return;
}

/////////////////////////////////////////////////////
// 要write的字串和ipStr傳入的都是std::string
/////////////////////////////////////////////////////
void socket_stream_write(int sockfd, std::string s, std::string ip_str, int clientNum)
{
        char ip_dot_decimal[INET_ADDRSTRLEN];
        strcpy(ip_dot_decimal, ip_str.c_str());
        socket_stream_write(sockfd, s, ip_dot_decimal, clientNum);
        return;
}

// ////////////////////////////////////////////////////
// // 要write的是c_str，傳入的ip是std::string
// ////////////////////////////////////////////////////
// void socket_stream_write(int sockfd, char reply[],std::string ip_str,int clientNum)
// {

// }

} // end of namespace "socket_stream_write"
#endif