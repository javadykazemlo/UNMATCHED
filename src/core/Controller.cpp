#include <iostream>
#include <string>
#include <iomanip>
#include "controller.hpp"

using namespace std;

Controller::Controller()
{
}



void Controller::calculation_cart(vector<Card>& deck,int index,int attack,int defend)
{
    if (attack > defend)
    {
        cout << "Attacker wins.\n";
        cout << "Damage ---> " << attack - defend << endl;
    }
    else if (attack < defend)
    {
        cout << "Defender blocks the attack.\n";
    }
    else
    {
        cout << "Equal attack and defense.\n";
    }
    if (index < 0 || index >= deck.size())
    {
        cout << "Invalid card index!\n";
        return;
    }
    burncards.push_back(deck[index]);

    deck.erase(deck.begin() + index);
}


vector<Card> Controller::get_burn_Cards() const
{
    return burncards;
}




Controller::~Controller()
{
}