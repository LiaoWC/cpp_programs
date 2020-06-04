#ifndef _input_processing_hpp_
#define _input_processing_hpp_

#include "headers.hpp"
#include "socket_stream_write.hpp"

namespace input_processing
{

        /*  default:
       you can type:
       alphabet, number, \0, \n, \r, space
      Here you can increase allowed char*/
        bool if_not_contain_invalid_char(char cstr[], std::string extraAllowChar)
        {
                bool ifValid = true;
                /* num is the number that you want to add to the extraAllowChar list */
                int num = extraAllowChar.length();
                // defalt
                if (num == 0)
                {
                        for (int i = 0; (unsigned)i < strlen(cstr); i++)
                        {
                                char c = cstr[i];
                                if (!(isalnum(c) || c == ' ' || c == '\0' || c == '\n' || c == '\r'))
                                {
                                        ifValid = false;
                                        break;
                                }
                        }
                }
                else if (num > 0)
                {
                        for (int i = 0; (unsigned)i < strlen(cstr); i++)
                        {
                                bool ifHaveMatch = false;
                                char c = cstr[i];
                                if (isalnum(c) || c == ' ' || c == '\0' || c == '\n' || c == '\r')
                                {
                                        ifHaveMatch = true;
                                }
                                else
                                {
                                        for (int j = 0; j < num; j++)
                                        {
                                                if (c == extraAllowChar[j])
                                                {
                                                        ifHaveMatch = true;
                                                        break;
                                                }
                                        }
                                }
                                if (!ifHaveMatch)
                                {
                                        ifValid = false;
                                        break;
                                }
                        }
                }
                else
                {
                }
                return ifValid;
        }

        /* This is still need to write "continue" by yourself. */
        bool input_check_handling(char cstr[], std::string extraAllowChar, char whenDoingWhat[], int sockfd, char ip_dot_decimal[], int clientNum)
        {
                std::string str_doing(whenDoingWhat);
                std::string str_contain_invalid_character_reply("Your input has invalid character. ");
                //str_contain_invalid_character_reply += "When ";
                str_contain_invalid_character_reply += str_doing;
                str_contain_invalid_character_reply += " ,you can only input:\nuppercase/lowercase english letters,\ndigit number,\nspace\n";
                bool rt = if_not_contain_invalid_char(cstr, extraAllowChar);
                if (rt == false)
                {
                        for (int i = 0; (unsigned)i < (unsigned)extraAllowChar.length(); i++)
                        {
                                str_contain_invalid_character_reply += "\"";
                                str_contain_invalid_character_reply += extraAllowChar[i];
                                str_contain_invalid_character_reply += "\"\n";
                        }
                        char *cstr = new char[(unsigned)str_contain_invalid_character_reply.length()];
                        strcpy(cstr, str_contain_invalid_character_reply.c_str());
                        socket_stream_write::socket_stream_write(sockfd, cstr, ip_dot_decimal, clientNum);
                }
                return rt;
        }

        /* This is still need to write "continue" by yourself. */
        bool input_check_handling_for_clientCMD(std::string str, std::string extraAllowChar, char whenDoingWhat[], int sockfd, char ip_dot_decimal[], int clientNum)
        {
                char *cstr = new char[(unsigned)str.size() + 1];
                strcpy(cstr, str.c_str());
                std::string str_doing(whenDoingWhat);
                std::string str_contain_invalid_character_reply("1 <spr> print Your input has invalid character. ");
                //str_contain_invalid_character_reply += "When ";
                str_contain_invalid_character_reply += str_doing;
                str_contain_invalid_character_reply += " ,you can only input:\nuppercase/lowercase english letters,\ndigit number,\nspace\n";
                bool rt = if_not_contain_invalid_char(cstr, extraAllowChar);
                if (rt == false)
                {
                        for (int i = 0; (unsigned)i < extraAllowChar.length(); i++)
                        {
                                str_contain_invalid_character_reply += "\"";
                                str_contain_invalid_character_reply += extraAllowChar[i];
                                str_contain_invalid_character_reply += "\"\n";
                        }
                        str_contain_invalid_character_reply += " <spr>";
                        char *cstr = new char[(unsigned)str_contain_invalid_character_reply.length()];
                        strcpy(cstr, str_contain_invalid_character_reply.c_str());
                        socket_stream_write::socket_stream_write(sockfd, cstr, ip_dot_decimal, clientNum);
                }
                delete cstr;
                return rt;
        }

        /* input_check_handling 的 std::string 版本 */
        bool input_check_handling(std::string str, std::string extraAllowChar, char whenDoingWhat[], int sockfd, char ip_dot_decimal[], int clientNum)
        {
                char *cstr = new char[(unsigned)str.length() + 1];
                strcpy(cstr, str.c_str());
                return input_check_handling(cstr, extraAllowChar, whenDoingWhat, sockfd, ip_dot_decimal, clientNum);
        }

}; // end of namespace input_processing
#endif