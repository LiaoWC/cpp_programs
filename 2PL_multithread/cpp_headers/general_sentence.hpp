#ifndef _general_sentence_hpp_
#define _general_sentence_hpp_

#include "headers.hpp"

namespace general_sentence
{

std::string lineMaker(char sigleChar, int num)
{
        std::string s = "";
        std::string stringC(1, sigleChar);
        while (num--)
                s += stringC;
        return s;
}

} // namespace general_sentence

#endif