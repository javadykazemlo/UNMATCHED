#include <iostream>
#include <string>
#include <vector>
#include "Sister.hpp"

using namespace std;

Sister::Sister(string name , int owner): Character(name , 1 , 2 , 0 , owner)
{
}

void Sister::ability(Bord* bord , Player* player)
{
    cout << "\n " << getName() << " has no special ability." << endl;
    cout << "   Sisters are Dracula's loyal followers who fight for him." << endl;
}

Sister::~Sister()
{
}