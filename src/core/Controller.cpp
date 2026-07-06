#include <iostream>
#include <string>
#include <iomanip>
#include "Controller.hpp"

using namespace std;

Controller::Controller()
{
}



void Controller::resolveCombat(Card& attackCard, Card& defenseCard, Player& attacker, Player& defender) 
{
    
    int attackValue = 0;
    int defenseValue = 0;
    
    if (attackCard.type == "Attack" || attackCard.type == "Versatile") 
    {
        attackValue = attackCard.attack;
    } 
    else 
    {
        attackValue = attackCard.boost;
    }
    
    if (defenseCard.type == "Defense" || defenseCard.type == "Versatile") 
    {
        defenseValue = defenseCard.defense;
    } 
    else 
    {
        defenseValue = defenseCard.boost;
    }
    
    cout << "\n═══════════════════════════════════════════════════════\n";
    cout << "  ⚔️ RESOLVING COMBAT ⚔️\n";
    cout << "═══════════════════════════════════════════════════════\n";
    
    cout << "\n🔹 ATTACKER (" << attacker.getName() << "):\n";
    attacker.getDeck()->showCard(attackCard);
    
    cout << "=======================================================================";

    cout << "\n🔸 DEFENDER (" << defender.getName() << "):\n";
    defender.getDeck()->showCard(defenseCard);
    
    cout << "\n📊 COMBAT RESULT:\n";
    cout << "  ⚔️ Attack  : " << attackValue << "\n";
    cout << "  🛡️ Defense : " << defenseValue << "\n";
    
    if (attackValue > defenseValue) 
    {
        int damage = attackValue - defenseValue;
        cout << "\n💥 " << attacker.getName() << " deals " << damage << " damage!\n";
        defender.getHero()->takeDamage(defenseValue, attackValue);
        cout << "  " << defender.getName() << " HP: " << defender.getHero()->getHp() 
        << "/" << defender.getHero()->getMaxhp() << "\n";
    }
    else if (attackValue < defenseValue) 
    {
        cout << "\n🛡️ " << defender.getName() << " blocks the attack!\n";
    } 
    else 
    {
        cout << "\n⚖️ Equal values! No damage.\n";
    }
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


void Controller::startCombat(Player& attacker, Player& defender)
{
    cout << "\n" << attacker.getName() << " - Select ATTACK card:\n";
    attacker.getDeck()->showHandSH();
    
    int chooseDR;
    Card* attackCard = nullptr;
    do {
        cout << "Enter card number (1-" << attacker.getDeck()->getHandSHSize() << "): ";
        cin >> chooseDR;
        attackCard = attacker.getDeck()->selectAndRemoveFromHandSH(chooseDR - 1);
        if (attackCard == nullptr) {
            cout << "Invalid choice! Try again.\n";
        }
    } 
    while (attackCard == nullptr);



    cout << "\n" << defender.getName() << " - Select DEFENSE card:\n";
    defender.getDeck()->showHandDR();
    
    int chooseSH;
    Card* defenseCard = nullptr;
    do {
        cout << "Enter card number (1-" << defender.getDeck()->getHandDRSize() << "): ";
        cin >> chooseSH;
        defenseCard = defender.getDeck()->selectAndRemoveFromHandDR(chooseSH - 1);
        if (defenseCard == nullptr) {
            cout << "Invalid choice! Try again.\n";
        }
    } 
    while (defenseCard == nullptr);


    resolveCombat(*attackCard, *defenseCard, attacker, defender);
    
    burncards.push_back(*attackCard);
    burncards.push_back(*defenseCard);
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