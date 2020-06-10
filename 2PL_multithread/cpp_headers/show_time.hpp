#ifndef _show_time_hpp_
#define _show_time_hpp_

#include "headers.hpp"

char *get_time_cstr_newChar()
{
        time_t rawtime;
        struct tm *timeInfo;
        //char *cstr = new char[100];
        char cstr[100];
        time(&rawtime);
        timeInfo = localtime(&rawtime);
        strftime(cstr, sizeof(cstr), "%F", timeInfo);

        std::string str(cstr);
        char *rt = new char[str.length() + 1];
        strcpy(rt, str.c_str());
        return rt;
}

#endif