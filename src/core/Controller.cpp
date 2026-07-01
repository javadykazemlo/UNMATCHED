#include <iostream>
#include <string>
#include <iomanip>
#include "controller.hpp"

using namespace std;

Controller::Controller()
{
}



void Controller::Resolve_Combat(vector<Card>& deck,int index,int attack,int defend)
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


void Controller::showBurnCards() const
{
    if (burncards.empty())
    {
        cout << "Burn pile is empty.\n";
        return;
    }

    cout << "\n===== Burn Cards =====\n";

    for (int i = 0; i < burncards.size(); i++)
    {
        cout << "Card " << i + 1 << endl;
        cout << "Name    : " << burncards[i].name << endl;
        cout << "Owner   : " << burncards[i].owner << endl;
        cout << "Type    : " << burncards[i].type << endl;
        cout << "Attack  : " << burncards[i].attack << endl;
        cout << "Defense : " << burncards[i].defense << endl;
        cout << "Boost   : " << burncards[i].boost << endl;
        cout << "Effect  : " << burncards[i].effect << endl;
        cout << "--------------------------------------------------------------\n";
    }
}



void Controller::selectCard_SH()const
{





}



void Controller::selectCard_DR()const
{




}



bool Controller::end_game() const
{




    
}


vector<Card> Controller::get_burn_Cards() const
{
    return burncards;
}




Controller::~Controller()
{
}