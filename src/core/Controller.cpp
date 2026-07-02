#include <iostream>
#include <string>
#include <iomanip>
#include "Controller.hpp"

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


void dispaly_card()
{




}






void Controller::plaseSidekicks(int cha , Bord bord)
{
    vector<int> zon;
    vector<int> space;
    int s;
    switch(cha)
    {
        case 1:
            zon = bord.getZone("Dracula");
            space = bord.getCellThisZone(zon);
            for(int i = 0 ; i < 3 ; i++)
            {
                cout << endl << "Available space: " << endl;
                for(int j = 0 ; j < zon.size()  ; j++)
                {
                    if(!bord.getSpaceStatus(i))
                        cout << "   " << zon[j];  
                }
                cout << "\nwhere do you want to place the " << i + 1 << "sister: ";
                cin >> s;
                for(int j = 0 ; j < zon.size()  ; j++)
                {
                    if(!bord.getSpaceStatus(s) && s == zon[j])
                        bord.addCharacter( s , "sister" );
                }

            }


            break;

        case 2:
            zon = bord.getZone("sherlock");
            space = bord.getCellThisZone(zon);

            cout << endl << "Available space: " << endl;
            for(int j = 0 ; j < zon.size()  ; j++)
            {
                if(!bord.getSpaceStatus(j))
                    cout << "   " << zon[j];  
            }
            cout << "\nwhere do you want to place the Dr_watson: ";
            cin >> s;
            for(int j = 0 ; j < zon.size()  ; j++)
            {
                if(!bord.getSpaceStatus(s) && s == zon[j])
                    bord.addCharacter( s , "Dr_watson" );
            }
            break;
    }

}



bool Controller::end_game(Player& p1, Player& p2) const
{
    Character* hero1 = p1.getHero();
    Character* hero2 = p2.getHero();

    if (!hero1->checkalive() || !hero2->checkalive())
    {
        cout << "GAME OVER\n";

        if (!hero1->checkalive())
            cout << hero2->getName() << " wins!\n";
        else
            cout << hero1->getName() << " wins!\n";

        return true;
    }

    return false;
}


vector<Card> Controller::get_burn_Cards() const
{
    return burncards;
}


Controller::~Controller()
{

}