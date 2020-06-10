#ifndef _TASK_HPP_
#define _TASK_HPP_

// C++
#include <vector>
#include <string>
#include "cpp_headers/string_manipulation.hpp"
// define
#define IF_DEBUG true
#define IF_FORMAT_ABSOLUTELY_RIGHT false
// namespace using
using namespace std;

class Task
{
public:
        // con/destructor
        Task(string instruction, int taskID);
        // member
        int wVar;
        int constant;
        vector<int> addVar;
        vector<int> subVar;
        vector<int> rVars;
        int taskID;
        // functions
        int varStr_to_varInt(string varStr);
        void printTaskInfo() const;
};

Task::Task(string instruction, int taskID)
{
        this->taskID = taskID;
        
        // split by space
        vector<string> words = string_manipulation::string_split_by_space(instruction);

        // check if the instruction argument number > 2
        if (words.size() <= 2)
        {
                cout << "ERROR-the number of words in the instruction is less than 3." << endl;
                exit(EXIT_FAILURE);
        }

        // words[0] => varStr
        this->wVar = varStr_to_varInt(words[0]);
        // words[1] => '='
        // words[2] => constant or variable

        int sumConstant = 0;
        if (string_manipulation::if_string_is_int(words[2])) // is int
                sumConstant += string_manipulation::string_to_int(words[2]);
        else // is not int
                this->addVar.push_back(varStr_to_varInt(words[2]));

        // the rest of words
        for (int i = 3; (unsigned)i < words.size(); i = i + 2)
        {
                char sign = words[i][0];
                string num = words[i + 1];
                bool isVar = true;
                if (string_manipulation::if_string_is_int(num))
                        isVar = false;

                if (isVar)
                {
                        if (sign == '+')
                                this->addVar.push_back(varStr_to_varInt(num));
                        else
                                this->subVar.push_back(varStr_to_varInt(num));
                }
                else
                {
                        if (sign == '+')
                                sumConstant += string_manipulation::string_to_int(num);
                        else
                                sumConstant -= string_manipulation::string_to_int(num);
                }
                this->constant = sumConstant;
        }

        /* find all var needed to be readLocked */
        this->rVars.push_back(this->wVar);
        for (auto item : this->addVar)
        {
                if (find(this->addVar.begin(), this->addVar.end(), item) == this->addVar.end())
                        this->rVars.push_back(item);
        }
        for (auto item : this->subVar)
        {
                if (find(this->subVar.begin(), this->subVar.end(), item) == this->subVar.end())
                        this->rVars.push_back(item);
        }
}

int Task::varStr_to_varInt(string varStr)
{
        // if the input format is absolutely right, this can be turned off to improve the performance
        if (IF_FORMAT_ABSOLUTELY_RIGHT == false)
        {
                // rm front back space
                varStr = string_manipulation::string_rm_front_behind_space(varStr);

                // check if a '$' with a integer
                if (varStr.length() < 2)
                {
                        cout << "ERROR-the length of std::string varStr  is less than 2." << endl;
                        exit(EXIT_FAILURE);
                }
                if (varStr[0] != '$')
                {
                        cout << "ERROR-the first char of std::string varStr  is not dollar sign." << endl;
                        exit(EXIT_FAILURE);
                }
        }

        string varIntStr(varStr.begin() + 1, varStr.end());

        // if (IF_FORMAT_ABSOLUTELY_RIGHT == false)
        // {
        //         if (!string_manipulation::if_string_is_int(varIntStr))
        //         {
        //                 cout << "ERROR-the word is not in valid varaible format." << endl;
        //                 exit(EXIT_FAILURE);
        //         }
        // }

        int varInt = string_manipulation::string_to_int(varIntStr);
        return varInt;
}

void Task::printTaskInfo() const
{
        cout << "==========" << endl;
        cout << "wVar: " << this->wVar << endl;
        cout << "constant: " << this->constant << endl;
        cout << "addVars: ";
        for (auto &item : this->addVar)
                cout << item << " ";
        cout << endl
             << "subVars: ";
        for (auto &item : this->subVar)
                cout << item << " ";
        cout << endl
             << "rVars: ";
        for (auto &item : this->rVars)
                cout << item << " ";
        cout << endl;
        cout << "==========" << endl;
}

#endif