#include <iostream>
#include <string>
#include <vector>
#include "Dr_watson.hpp"

using namespace std;

Dr_watson::Dr_watson(): Character("Dr_watson", 8 , 2 , 0)
{
}

void Dr_watson::ability()
{
    cout << "\n " << getName() << " has no special ability." << endl;
    cout << "   Dr. Watson is Sherlock's loyal companion and fights alongside him." << endl;
}

Dr_watson::~Dr_watson()
{
}