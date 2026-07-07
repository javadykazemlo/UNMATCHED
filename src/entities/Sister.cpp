#include <iostream>
#include <string>
#include <vector>
#include "Sister.hpp"

using namespace std;

Sister::Sister(string name): Character(name , 1 , 2 , 0)
{
}

void Sister::ability()
{
    cout << "\n " << getName() << " has no special ability." << endl;
    cout << "   Sisters are Dracula's loyal followers who fight for him." << endl;
}

Sister::~Sister()
{
}