#include <iostream>
#include <string>
#include <algorithm>
#include "invisible_man.hpp"

using namespace std;

invisible_man::invisible_man(int owner): Character("invisible man", 15, 2, 0 , owner , true)
{
    mistTokens = {-1 , -1 , -1};
}

void invisible_man::ability(Bord bord , Player* player)
{
    cout << "\nInvisible Man's ability:\n";
    cout << " - While defending on a space with a fog token, his defense card value gains +1 (not a card effect, cannot be cancelled by Feint or similar).\n";
    cout << " - He can move directly between any two spaces that hold a fog token, as if they were adjacent.\n";

    if(isOnMist(getSpace()))
        cout << "He is currently standing on a fog token.\n";
    else
        cout << "He is currently not standing on a fog token.\n";
}

void invisible_man::setMistToken(int index , int pos)
{
    if(index >= 0 && index < (int)mistTokens.size())
        mistTokens[index] = pos;
}

int invisible_man::getMistToken(int index) const
{
    if(index >= 0 && index < (int)mistTokens.size())
        return mistTokens[index];
    return -1;
}

vector<int> invisible_man::getMistTokens() const
{
    return mistTokens;
}

bool invisible_man::isOnMist(int pos) const
{
    return isMistPosition(pos);
}

bool invisible_man::isMistPosition(int pos) const
{
    if(pos == -1)
        return false;
    return find(mistTokens.begin() , mistTokens.end() , pos) != mistTokens.end();
}