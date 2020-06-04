#ifndef _FILE_MANIPULATION_HPP_
#define _FILE_MANIPULATION_HPP_
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
using namespace std;

class file_manipulation
{
public:
        static int fileSize(string fileName);
};

/* return the fileSize of the fileName in bytes
++Warning++ It may overflow if number is too large. */
int file_manipulation::fileSize(string fileName)
{
        struct stat stat_buffer;
        if (stat(fileName.c_str(), &stat_buffer) != 0)
        {
                cout << "ERROR-get stat of the file" << endl;
        }
        int fileSize = stat_buffer.st_size;
        return fileSize;
}

#endif