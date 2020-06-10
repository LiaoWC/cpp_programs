#ifndef _STRING_MANIPULATION_HPP_
#define _STRING_MANIPULATION_HPP_

#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
using namespace std;

namespace string_manipulation
{
        /* remove all the single specific char from the string */
        void string_rm_allSingleChar(string &str, char singleChar)
        {
                str.erase(std::remove(str.begin(), str.end(), singleChar), str.end());
                return;
        }

        /* remove all the specific chars from the string */
        void string_rm_allChars(string &str, string rmChars)
        {
                for (auto &c : rmChars)
                        string_manipulation::string_rm_allSingleChar(str, c);
                return;
        }

        /* remove all the specific chars from the string */
        void string_rm_allChars(string &str, char *rmCharsCstr, int strLen)
        {
                string rmChars(rmCharsCstr, rmCharsCstr + strLen);
                string_manipulation::string_rm_allChars(str, rmChars);
                return;
        }

        char *string_to_newChar_needDeleted(string str)
        {
                int len = (unsigned)str.size() + 1;
                char *cstr = new char[len];
                strcpy(cstr, str.c_str());
                return cstr;
        }

        /* 把送進來的 string 按照第一個空白切成兩段存在vector的兩格 */
        /* 第一和第二個字串中間原本的空格會拿掉，換行符號也會拿掉　*/
        /* 最前面的空白會被拿掉 */
        // 備註：若沒有第一個空白就第二格放空字元
        std::vector<std::string> string_cut_in_half(std::string str)
        {
                std::string first;
                std::string second;
                bool hadSeenNotSpace = false;
                bool hadSeenValidSpace = false;
                for (int i = 0; (unsigned)i < str.length(); i++)
                {
                        char temp = str[i];
                        // 前面的空白不收
                        if (hadSeenNotSpace == false && (temp != ' ' /* || temp != '\n' || temp != '\r'*/))
                                hadSeenNotSpace = true;

                        //// 不收換行符號
                        // if (temp == '\n' || temp == '\r')
                        //         continue;

                        if (temp == ' ' && hadSeenValidSpace == false)
                        {
                                // 還沒收過非空白以前不收空白
                                if (hadSeenNotSpace == false)
                                        continue;
                                else
                                {
                                        hadSeenValidSpace = true;
                                        continue;
                                }
                        }

                        // 判斷要放到first還是second
                        if (hadSeenValidSpace)
                                second += temp;
                        else
                                first += temp;
                }
                std::vector<std::string> rt(2);
                rt[0] = first;
                rt[1] = second;
                return rt;
                // 轉cstring to std::string
                // char *cstr = new char [(unsigned)str.length() + 1];
                // strcpy(cstr,str.c_str());
                // return cstring_cut_in_half(cstr);
        }

        /*　把送進來的 c_string 按照第一個空白切成兩段存在vector的兩格　*/
        // 備註：若沒有第一個空白就第二格放空字元
        std::vector<std::string> cstring_cut_in_half(char cstr[])
        {
                std::string str(cstr);
                return string_cut_in_half(str);
        }

        /* 將 string 全部依空格切開，每個新字串頭尾不會有空格 */
        std::vector<std::string> string_split_by_space(std::string str)
        {
                std::vector<std::string> rt;
                std::string temp;
                for (int i = 0; (unsigned)i < str.length(); i++)
                {
                        if (str[i] == ' ')
                        {
                                if ((unsigned)temp.length() > 0)
                                {
                                        rt.emplace_back(temp);
                                        temp.clear();
                                }
                                continue;
                        }
                        temp += str[i];
                }
                if ((unsigned)temp.length() > 0)
                        rt.emplace_back(temp);
                return rt;
        }
        /* 將 c_string 全部依空格切開，每個新字串頭尾不會有空格 */
        std::vector<std::string> cstring_split_by_space(char cstr[])
        {
                std::string str(cstr);
                return string_split_by_space(str);
        }

        /* 偵側輸入字串是否全空或全為空白、\n、\r */
        bool string_if_empty_or_allSpaceAndOther(std::string str)
        {
                if ((unsigned)str.length() == 0)
                        return true;
                bool flag = true;
                for (int i = 0; (unsigned)i < str.length(); i++)
                {
                        char curChar = str[i];
                        if (curChar != ' ' || curChar != '\n' || curChar != '\r')
                        {
                                flag = false;
                                break;
                        }
                }
                return (flag == true);
        }

        char *string_to_asciiCstr(std::string str)
        {
                std::string tempContainer = " ";
                for (int i = (unsigned)str.length() - 1; i >= 0; i--)
                {
                        int ascNum = (int)str[i];
                        while (ascNum > 0)
                        {
                                int digit = ascNum % 10;
                                ascNum = (ascNum - digit) / 10;
                                tempContainer += (char)((int)'0' + digit);
                        }
                        if (i != 0)
                                tempContainer += " ";
                }
                std::reverse(tempContainer.begin(), tempContainer.end());
                char *ptr = new char[(unsigned)tempContainer.length() + 1];
                strcpy(ptr, tempContainer.c_str());
                return ptr;
        }

        std::string string_to_asciiStr(std::string str)
        {
                char *ptr = string_to_asciiCstr(str);
                std::string rt(ptr);
                delete ptr;
                return rt;
        }

        /* 由於在很多地方會把字串前後的空格去除，所以這個轉換在設計上能符合需求 */
        char *asciiCstr_to_cstr(char asciiCstr[])
        {
                std::string tempContainer;
                int curNum = 0;
                for (int i = 0; (unsigned)i < strlen(asciiCstr); i++)
                {
                        if (asciiCstr[i] == ' ')
                        {
                                tempContainer += (char)curNum;
                                curNum = 0;
                                continue;
                        }
                        curNum = curNum * 10 + ((int)asciiCstr[i] - (int)'0');
                        if (((unsigned)i == (strlen(asciiCstr) - 1) && isdigit(asciiCstr[i])))
                        {
                                tempContainer += (char)curNum;
                                curNum = 0;
                                continue;
                        }
                }
                char *ptr = new char[(unsigned)tempContainer.length() + 1];
                strcpy(ptr, tempContainer.c_str());
                return ptr;
        }

        std::string asciiCstr_to_str(char asciiCstr[])
        {
                char *ptr = asciiCstr_to_cstr(asciiCstr);
                std::string str(ptr);
                delete[] ptr;
                return str;
        }

        std::string asciiStr_to_str(std::string asciiStr)
        {
                char *asciiCstr = new char[(unsigned)asciiStr.length() + 1];
                strcpy(asciiCstr, asciiStr.c_str());
                std::string rt = asciiCstr_to_str(asciiCstr);
                delete[] asciiCstr;
                return rt;
        }

        char *string_to_cstr(std::string str)
        {
                char *ptr = new char[(unsigned)str.length() + 1];
                strcpy(ptr, str.c_str());
                return ptr;
        }

        /* 移除std::string後面的空格 */
        std::string string_rm_behind_space(std::string str)
        {
                std::string tempContainer;
                std::reverse(str.begin(), str.end());
                bool ifHaveSeenNotSpaceOrOther = false;
                for (unsigned int i = 0; i < (unsigned)str.length(); i++)
                {
                        if (ifHaveSeenNotSpaceOrOther == false)
                        {
                                if (!(str[i] == ' ' /*||tr[i] == '\n' || str[i] == '\r'*/))
                                {
                                        ifHaveSeenNotSpaceOrOther = true;
                                        tempContainer += str[i];
                                }
                        }
                        else
                                tempContainer += str[i];
                }
                std::reverse(tempContainer.begin(), tempContainer.end());
                return tempContainer;
        }

        /* 移除std::string前後的空格 */
        std::string string_rm_front_behind_space(std::string str)
        {
                str = string_rm_behind_space(str);
                std::reverse(str.begin(), str.end());
                str = string_rm_behind_space(str);
                std::reverse(str.begin(), str.end());
                return str;
        }

        /* 依指定的詞切開std::string */
        std::vector<std::string> string_cut_by_word(std::string str, std::string word)
        {
                std::vector<std::string> rt;
                std::size_t foundPos = str.find(word);
                if (foundPos == std::string::npos)
                {
                        // 找不到的話，退回原本的str，傳回空vector
                        return rt;
                }
                std::string frontStr(str.begin(), str.begin() + foundPos);
                std::string behindStr(str.begin() + foundPos + (unsigned)word.length(), str.end());
                rt.push_back(frontStr);
                rt.push_back(behindStr);
                return rt;
        }

        void string_replace_all_with(std::string &str, std::string target, std::string replaceStr)
        {
                std::size_t pos = str.find(target);
                while (pos != std::string::npos)
                {
                        str.replace(pos, (unsigned)target.length(), replaceStr);
                        pos = str.find(target, pos + (unsigned)replaceStr.length());
                }
                return;
        }

        bool if_string_isdigit(std::string str)
        {
                return str.find_first_not_of("0123456789") == std::string::npos && (unsigned)str.length() != 0;
        }

        /*    看a,b兩個std::string在std::string str裡的排列順序
        若a存在，b不存在 => 回傳4
        若b存在，a不存在 => 回傳3
        若a,b皆不存在 => 回傳2
        若a,b皆存在，且a在b前面 => 回傳1
        若a,b皆存在，且b在a前面 => 回傳0
        若a,b相同 => 回傳-1
        若a是b的連續substring或b是a的連續substring回傳-2        */
        int two_string_appear_order(std::string str, std::string a, std::string b)
        {
                if (a == b)
                        return -1;
                if (strstr(a.c_str(), b.c_str()) != NULL || strstr(b.c_str(), a.c_str()))
                        return -2;
                std::size_t posA = str.find(a);
                std::size_t posB = str.find(b);
                if (posA == std::string::npos)
                {
                        if (posB == std::string::npos)
                                return 2;
                        else
                                return 3;
                }
                if (posB == std::string::npos)
                        return 4;
                return (posA < posB ? 1 : 0);
        }

        /* find bStr in aStr  */
        bool if_oneStrIngTheOtherStr(std::string aStr, std::string bStr)
        {
                std::size_t found = aStr.find(bStr);
                return (found != std::string::npos);
        }

        /* check if the given string is an integer */
        /* not guarantee about overflowing */
        /*  It's okay that any zero in the front part of the string */
        bool if_string_is_posInt(std::string str)
        {
                // check the length
                if (str.length() <= 0)
                        return false;

                // check if all char is digit
                bool allCharDigit = true;
                for (auto &c : str)
                {
                        if (!isdigit(c))
                        {
                                allCharDigit = false;
                                break;
                        }
                }
                if (!allCharDigit)
                        return false;

                // pass the checking
                return true;
        }

        /* can be negative or positive */
        bool if_string_is_int(std::string str)
        {
                if (str[0] == '-')
                {
                        string unsignStr(str.begin() + 1, str.end());
                        return (string_manipulation::if_string_is_posInt(unsignStr));
                }
                return string_manipulation::if_string_is_posInt(str);
        }

        /* Turn a char into a decimal digit */
        /* ++Warning++ please ensure it's actually an digit-char. */
        int char_to_digit(char c)
        {
                return ((int)c - (int)'0');
        }

        /* turn string into integer */
        /* please be sure that the string is actually an integer */
        /* ( You may use function "if_string_is_int" to check ) */
        /*  It's okay that any zero in the front part of the string */
        /* ++Warning++ please be sure it won't overflow in int data type */
        int string_to_int(std::string str)
        {
                int result = 0;
                bool ifNeg = false;

                //  if negative
                if (str[0] == '-')
                {
                        string newStr(str.begin() + 1, str.end());
                        str = newStr;
                        ifNeg = true;
                }

                for (int i = str.length() - 1; i >= 0; i--)
                {
                        int digit = string_manipulation::char_to_digit(str[i]);
                        result += (int)pow(10, str.length() - 1 - i) * digit;
                }

                if (ifNeg)
                        return (result * (-1));
                else
                        return result;
        }

} // namespace string_manipulation

#endif