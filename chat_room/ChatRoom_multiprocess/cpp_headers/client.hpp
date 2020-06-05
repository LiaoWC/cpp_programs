#ifndef _client_hpp_
#define _client_hpp_

#include "headers.hpp"
#include "input_processing.hpp"
#include "socket_stream_write.hpp"
#include "sqlite3_db.hpp"
#define end_read_string "<end@csnctuedutw@>" // this should be complicated, or the message may contain same strings, and it will cause the client read incorrectly.
#define read_loop_max_inClientHpp 10000
#define bufferSize_inClientHpp 2560
using namespace std;

class Client
{
private:
        int clientNum;
        std::string ipStr;
        bool ifLogin;
        int sockfd;
        std::string username;
        std::string fullCMD;     // 使用者下的一行指令
        std::string restCMD;     // fullCMD處理後剩下的
        std::string lastCutHead; // 上一次被做切最前得到的結果

public:
        /* constructor */
        /* set members */
        void set_clientNum(int clientNum);
        void set_ip_str(char ip_cstr[]);
        void set_ifLogin(bool ifLogin);
        void set_username(char curLoginUsername[]);
        void set_fullCMD(char buffer[]); // 同時restCMD也會被重置
        void set_fullCMD(std::string str);
        void set_restCMD(std::string newRestCMD);
        void set_sockfd(int sockfd);
        /* string processing */
        bool check_restCMD_dontHaveValidChar();
        int do_nextHeadCut();
        int count_restCMD_ItemsByDefault();
        /* get members */
        std::string get_lastCutHead() const;
        std::string get_restCMD() const;
        std::string get_fullCMD() const;
        bool get_ifLogin() const;
        int get_sockfd() const;
        std::string get_ipStr() const;
        char *get_ipStr_inCstr() const;
        int get_clientNum() const;
        std::string get_username() const;
        char *get_username_inCstr() const;
        /* do some operation */
        void do_logout();
        bool input_check_handling(char cstr[], std::string extraAllowChar, char whenDoingWhat[]) const;
        void socket_stream_write(char cstr[]);
        void socket_stream_write(std::string str);
        std::string readFromSocket();
        /* others */
        bool handle_checkIfLogin_beforeOperation();
};
//////////////
//     SET    //
//////////////

void Client::set_clientNum(int clientNum)
{
        this->clientNum = clientNum;
        return;
}

void Client::set_ip_str(char ip_cstr[])
{
        //char ip_cstr[INET_ADDRSTRLEN]
        std::string temp(ip_cstr);
        this->ipStr = temp;
        return;
}

void Client::set_ifLogin(bool ifLogin)
{
        this->ifLogin = ifLogin;
        return;
}

void Client::set_username(char curLoginUsername[])
{
        std::string temp(curLoginUsername);
        this->username = temp;
        return;
}

void Client::set_fullCMD(char buffer[])
{
        fullCMD.clear();
        restCMD.clear();
        std::string temp;
        for (int i = 0; i < strlen(buffer); i++)
        {
                if (buffer[i] == '\n' || buffer[i] == '\r')
                        continue;
                temp += buffer[i];
        }
        this->fullCMD = temp;
        this->restCMD = temp;
        return;
}

void Client::set_fullCMD(std::string str)
{
        char *ptr = new char[(unsigned)str.size() + 1];
        strcpy(ptr, str.c_str());
        set_fullCMD(ptr);
        delete ptr;
        return;
}

void Client::set_restCMD(std::string newRestCMD)
{
        this->restCMD = newRestCMD;
        return;
}

void Client::set_sockfd(int sockfd)
{
        this->sockfd = sockfd;
        return;
}

/////////////////////
//          DO             //
/////////////////////

bool Client::check_restCMD_dontHaveValidChar()
{
        return input_processing::string_if_empty_or_allSpaceAndOther(this->restCMD);
}

int Client::do_nextHeadCut()
{
        // 若成功return 0，若restCMD已經空了，return 1
        if (check_restCMD_dontHaveValidChar())
        {
                fprintf(stdout, "restCMD cannot be cut since it don't have valid chars.\a\n");
                return 1;
        }
        std::vector<std::string> temp = input_processing::string_cut_in_half(this->restCMD);
        this->lastCutHead = temp[0];
        this->restCMD = temp[1];
        return 0;
}

int Client::count_restCMD_ItemsByDefault()
{
        std::vector<std::string> temp = input_processing::string_split_by_space(restCMD);
        return temp.size();
}

/////////////////
//       GET       //
/////////////////

std::string Client::get_lastCutHead() const
{
        return lastCutHead;
}

std::string Client::get_restCMD() const
{
        return restCMD;
}

std::string Client::get_fullCMD() const
{
        return fullCMD;
}

bool Client::get_ifLogin() const
{
        return ifLogin;
}

int Client::get_sockfd() const
{
        return this->sockfd;
}

std::string Client::get_ipStr() const
{
        return this->ipStr;
}

char *Client::get_ipStr_inCstr() const
{
        char *ptr = new char[ipStr.length() + 1];
        strcpy(ptr, ipStr.c_str());
        return ptr;
}

int Client::get_clientNum() const
{
        return this->clientNum;
}

std::string Client::get_username() const
{
        return this->username;
}

char *Client::get_username_inCstr() const
{
        char *ptr = new char[username.length() + 1];
        strcpy(ptr, username.c_str());
        return ptr;
}

///////////////////////////////////////////
//         DO SOME OPERATIONS           //
///////////////////////////////////////////

void Client::do_logout()
{
        this->ifLogin = false;
        this->username.clear();
        return;
}

bool Client::input_check_handling(char cstr[], std::string extraAllowChar, char whenDoingWhat[]) const
{
        return input_processing::input_check_handling(cstr, extraAllowChar, whenDoingWhat, get_sockfd(), get_ipStr_inCstr(), get_clientNum());
}

void Client::socket_stream_write(char cstr[])
{
        /* project 3 之後要加上end_read_string */
        string temp(cstr);
        temp += " ";
        temp += end_read_string;
        char *new_cstr = new char[(unsigned)temp.size() + 1];
        strcpy(new_cstr, temp.c_str());

        socket_stream_write::socket_stream_write(get_sockfd(), new_cstr, get_ipStr_inCstr(), get_clientNum());
        delete new_cstr;
        return;
}

void Client::socket_stream_write(std::string str)
{
        char *cstr = new char[str.length() + 1];
        strcpy(cstr, str.c_str());
        socket_stream_write(cstr);
        delete[] cstr;
        return;
}

std::string Client::readFromSocket()
{
        /* 我自定義每次read都要到收到結束字串才能停止read，不然網路慢的話可能會出問題 */
        string readUntilNow;
        std::size_t found_pos;
        int i = 0;
        for (; i < read_loop_max_inClientHpp; i++)
        {

                char buffer[bufferSize_inClientHpp];
                bzero(buffer, bufferSize_inClientHpp);
                if (read(sockfd, buffer, bufferSize_inClientHpp) < 0)
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
        if (i == read_loop_max_inClientHpp)
        {
                /* it may be forever loop */
                fprintf(stderr, "ERROR: cannot find the end string of the messeage.\a\n");
        }
        restCMD = readUntilNow;
        return readUntilNow;
}

////////////// others /////////////
bool Client::handle_checkIfLogin_beforeOperation()
{
        if (!get_ifLogin())
        {
                char reply[] = "1 <spr> print Please login first.\n <spr>";
                socket_stream_write(reply);
                return false;
        }
        return true;
}

#endif