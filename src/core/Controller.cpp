#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include "Controller.hpp"

using namespace std;

Controller::Controller()
{
}

void Controller::choosePlayers(Player player[2])
{
    cout << "════════════════════════════════════════════════════════════════════\n";
    cout << "                 welcome to the game : (UNMATCHED)\n";
    cout << "════════════════════════════════════════════════════════════════════\n";

    string n;
    int a;

    cout << "First player, enter your name: ";
    getline(cin , n);
    player[0].setName(n);

    cout << player[0].getName() << ", enter your age: ";
    a = getInt();
    player[0].setAge(a);
    
    
    cout << "Second player, enter your name: ";
    getline(cin >> ws , n);
    player[1].setName(n);
    
    cout << player[1].getName() << ", enter your age: ";
    a = getInt();
    player[1].setAge(a);
    
    if(player[0].getAge() <= player[1].getAge())
    {
        firstPlayer = &player[0];
        secondPlayer = &player[1];
    }
    else
    {
        firstPlayer = &player[1];
        secondPlayer = &player[0];
    }
}


void Controller::chooseCharacters(Bord& bord)
{
    int cha;
	cout << "\nCharacters:" << endl;
    cout << " 1.Dracula" << endl;
    cout << " 2.Sherlock" << endl;

    cout << firstPlayer->getName() << ", choose your Character: ";
    cha = getChoice({1,2});

    firstPlayer->chooseCharacter( cha );
    secondPlayer->chooseCharacter( cha == 1 ? 2 : 1 );



    int pos[2];
    cout << firstPlayer->getName() << ", choose your Character position(4 or 15): ";
    pos[0] = getChoice({4,15});

    bord.addCharacter( pos[0] , firstPlayer->getHero() );
    bord.addCharacter( pos[0] == 4 ? 15 : 4 , secondPlayer->getHero() );


    plaseSidekicks(bord , *firstPlayer);
    plaseSidekicks(bord , *secondPlayer);


    cout << firstPlayer->getName() << " chose " << firstPlayer->getHero()->getName() << " and starts at spaces " << pos[0] << ".\n"; 
    cout << secondPlayer->getName() << " chose " << secondPlayer->getHero()->getName() << " and starts at spaces " << pos[1] << ".\n";  

}



void Controller::plaseSidekicks( Bord& bord , Player& player)
{
    Character* hero = player.getHero();
    vector<int> zone = bord.getZone(hero);
    vector<int> space  = bord.getCellThisZone(zone);

    int s;
    
    if(hero->getName() == "Dracula")
    {
        for(int i = 0 ; i < 3 ; i++)
        {
            cout << endl << "Available space: " << endl;
            for(int j = 0 ; j < space.size()  ; j++)
            {
                if(!bord.getSpaceStatus(space[j]))
                    cout << "   " << space[j];  
            }
            cout << "\nwhere do you want to place the " << i + 1 << "sister: ";
            s = getInt();
            for(int j = 0 ; j < space.size()  ; j++)
            {
                if(!bord.getSpaceStatus(space[j]) && s == space[j])
                    bord.addCharacter( s, player.getsidekick(i+1));
            }
    
        }
    }
    else
    {
        cout << endl << "Available space: " << endl;
        for(int j = 0 ; j < space.size()  ; j++)
        {
            if(!bord.getSpaceStatus(space[j]))
                cout << "   " << space[j];  
        }
        cout << "\nwhere do you want to place the Dr_watson: ";
        s = getInt();
        for(int j = 0 ; j < space.size()  ; j++)
        {
            if(!bord.getSpaceStatus(space[j]) && s == space[j])
                bord.addCharacter( s, player.getsidekick(1) );
        }

    }
    
}

void Controller::playTurn(Bord& bord)
{


}


void move(Bord& bord)
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





int Controller::getInt()
{
    int x;

    while (true)
    {
        cin >> x;

        if (!cin.fail())//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        {
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return x;
        }

        cout << "Invalid input. Enter a number: ";

        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int Controller::getChoice(vector<int> valid)
{
    while(true)
    {
        int x = getInt();

        for(int i : valid)
        {
            if(i == x) return x;
        }

        cout << "Invalid choice. Try again: ";
    }
}


bool Controller::end_game() const
{
    Character* hero1 = firstPlayer->getHero();
    Character* hero2 = secondPlayer->getHero();

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