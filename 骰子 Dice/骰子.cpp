#include<iostream>
#include<cstdlib>
#include<ctime>
#include<cctype>
#include<string>
#include<cmath>

using namespace std;

void throwTheDice(int diceNum)
{
    int sum = 0;
    for(int i=0;i<diceNum;i++)
    {
        sum += ((rand()%6)+1);
    }
    int digitNum = (int)log10(sum) + 1;

    //cout <<"log " << sum << " " << log10(sum) << " " << digitNum << endl;

    cout << "@@";
    for(int i=0;i<digitNum;i++)
        cout << "@";
    cout << "@@";
    cout << endl;

    cout << "@ " << sum << " @" << endl;

    cout << "@@";
    for(int i=0;i<digitNum;i++)
        cout << "@";
    cout << "@@";
    cout << endl;

}
int switchNumDice()
{
    cout << "Please input the number of dice that you want:";
    string inputString;
    cin >> inputString;
    fflush(stdin);
    bool validInput = true;
    while( true )
    {
        if(inputString.size()>8)
            validInput = false;
        for(int i=0;i<inputString.size();i++)
        {
            if(!isdigit(inputString[i]))
            {
                validInput = false;
                break;
            }
        }
        if( validInput )
            break;
        cout << "WRONG INPUT. Please input again." << endl;
        cin >> inputString;
        validInput = true;
        fflush(stdin);
    }
    int number = 0;
    for(int i=0;i<inputString.size();i++)
    {
        number *= 10;
        number += (int)inputString[i] - (int)'0';
    }
    return number;
}
void seeNumDice(int diceNum)
{
    cout << "The number of dices now is " << diceNum << "." << endl;
    return;
}

int main()
{
    srand(( unsigned) time(NULL));
    int diceNum = 1;
    bool ifContinue = true;
    while(ifContinue)
    {

        cout << "==========" << endl;
        cout << "Input message:" << endl << "1 is throw the dice;" << endl;
        cout << "2 is change the number of dice;" << endl;
        cout << "3 is to see the number of dice now;" << endl;
        cout << "4 is to terminate this program;" << endl;
        cout << "==========" << endl;
        char key = '1';
        string temp;
        cin >> temp;
        while(temp.size()!=1 && isdigit(temp[0]))
        {
            cout << "WRONG INPUT. Please input again." << endl;
            cin >> temp;
            fflush(stdin);
        }
        key = temp[0];

        while(key!='1' && key!='2' && key!='3' && key!='4')
        {
            cout << "WRONG INPUT, PLEASE INPUT AGAIN" << endl;
            //fflush(stdin);
            cin >> key;
            fflush(stdin);
        }
        switch(key)
        {
        case '1':
            throwTheDice(diceNum);
            break;
        case '2':
            diceNum = switchNumDice();
            break;
        case '3':
            seeNumDice(diceNum);
            break;
        case '4':
            cout << "end" << endl;
            system("PAUSE");
            ifContinue = false;
            break;
        }
    }
    return 0;
}
