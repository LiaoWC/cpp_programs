// reference: https://stackoverflow.com/questions/5605125/why-is-iostreameof-inside-a-loop-condition-i-e-while-stream-eof-cons
// This program will replace the destination-file if it already exists before you run this program.

#include <iostream>
#include <fstream>
// #define filePath_src "Src"
// #define filePath_dest "Dest"

int main(int argc, char *argv[])
{
        if (argc != 3)
        {
                std::cout << "You have to type source-file-name and destination-file-name." << std::endl;
                return 0;
        }

        const char *src = argv[1];
        const char *dest = argv[2];

        std::ifstream file_src(src, std::fstream::binary);
        std::ofstream file_dest(dest, std::fstream::binary | std::fstream::trunc);

        file_dest << file_src.rdbuf();

        file_src.close();
        file_dest.close();
        std::cout << "Done!" << std::endl;
        return 0;
}
