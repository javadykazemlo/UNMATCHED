#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include "Controller.hpp"

using namespace std;

Controller::Controller()
{
    DraculaWon = false;
    sherlockWon = false;
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

    current = firstPlayer;
    enemy = secondPlayer;

}


void Controller::chooseCharacters(Bord& bord)
{
    int cha;
	cout << "\nCharacters:" << endl;
    cout << " 1.Dracula" << endl;
    cout << " 2.Sherlock" << endl;

    cout << firstPlayer->getName() << ", choose your Character: ";
    cha = getChoice({1,2});


    if (cha == 1) 
    {
        secondPlayer->chooseCharacter(2);  
    } 
    
    else 
    {
        secondPlayer->chooseCharacter(1);  
    }

    int pos;
    cout << firstPlayer->getName() << ", choose your Character position(4 or 15): ";
    pos = getChoice({4,15});

    bord.addCharacter( pos , firstPlayer->getHero() );
    bord.addCharacter( pos == 4 ? 15 : 4 , secondPlayer->getHero() );


    plaseSidekicks(bord , *firstPlayer);
    plaseSidekicks(bord , *secondPlayer);
}



void Controller::plaseSidekicks( Bord& bord , Player& player)
{
    Character* hero = player.getHero();
    vector<int> zone = bord.getZone(hero);
    vector<int> space  = bord.getCellThisZone(zone);

    int s;
    
    if(hero->getName() == "Dracula")
    {
        cout << "\n═════════════════════════════════════════════════" << endl;
        for(int i = 0 ; i < 3 ; i++)
        {
            cout << "Available space: ";
            for(int j = 0 ; j < space.size()  ; j++)
            {
                if(!bord.getSpaceStatus(space[j]))
                    cout << "   " << space[j];  
            }
            cout << "\nwhere do you want to place sister #" << i + 1 << ": ";
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
        cout << "\n═════════════════════════════════════════════════" << endl;
        cout << "Available space: ";
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
    int charc = 0;
    vector<int> chact = {};
    int Todo = 0;
    while(!end_game())
    {
        for(int i = 0; i < 2 ; i++)
        {
            cout << "\n════════════════════════════════════════════════════════════════" << endl;
            cout << "            " << current->getName() << "'s turn\n\n";
    
            // نمایش TUI (نقشه و اطلاعات کاراتر ها و اطلاعات کارت ها)
    
            cout << "\nActions:  \n 1.Maneuver\n 2.Scheme\n 3.Attack";
            cout << "\nChoose a action: ";
            Todo = getChoice({1,2,3});
            
            int k = 1;
            
            switch(Todo)
            {
                case 1: //Maneuver
                {
                    //کشیدن یک کارت

                    for (Character* ch : current->getCharacters())
                    {
                        if(ch->checkalive())
                        {
                            cout << k << "." << ch->getName() << endl;
                            chact.push_back(k);
                            k++;
                        }
                    }
                    cout << "Choose a character: ";
                    charc = getChoice({chact});
    
                    move(bord , charc);
    
                    break;
    
                }
                case 2: //Scheme
                {
                    //انتخاب یکی از کارت های Scheme
    


                    break;
    
                }
                case 3: //Attack
                {
                    int k = 1;
                    for (Character* ch : current->getCharacters())
                    {
                        if(ch->checkalive())
                        {
                            cout << k << "." << ch->getName() << endl;
                            chact.push_back(k);
                            k++;
                        }
                    }
                    cout << "Choose a character: ";
                    charc = getChoice({chact});
    
                    
                    break;
    

                }
                default:
                {
                    break;
                }
            }
            move(bord , 2);
    
            // حمله
            //attack();
    
            if(end_game()) 
            {
                break;
                
            }
    
            // کشیدن کارت
            //drawCard();

        }

        swap(current, enemy);
    }

}


void Controller::move(Bord& bord , int ch)
{

}



void Controller::boost(Card*& selectedCard, bool isAttackCard, bool isDefenseCard)
{
    if (selectedCard == nullptr)
    {
        cout << "No card selected!\n";
        return;
    }

    cout << "\nSelected card: " << selectedCard->name << endl;
    cout << "Boost value: " << selectedCard->boost << endl;
    
    cout << "Do you want to use Boost? (y/n): ";
    char choice;
    cin >> choice;

    if (choice != 'y' && choice != 'Y')
    {
        cout << "No boost used.\n";
        return;
    }

    cout << "\nHow do you want to use the Boost?\n";
    cout << "   1. For Movement (+" << selectedCard->boost << " steps)\n";
    
    if (isAttackCard)
        cout << "   2. For Attack (+" << selectedCard->boost << " attack)\n";
    
    if (isDefenseCard)
        cout << "   3. For Defense (+" << selectedCard->boost << " defense)\n";
    
    cout << "   0. Cancel Boost\n";
    cout << "Enter choice: ";

    int boostChoice;
    cin >> boostChoice;

    if (boostChoice == 0)
    {
        cout << "Boost cancelled.\n";
    }
    else if (boostChoice == 1)  // حرکت
    {
        cout << "\n✅ Boost used for MOVEMENT!\n";
        cout << "   +" << selectedCard->boost << " steps added.\n";
        // مقدار Boost برای حرکت ذخیره میشه (در move استفاده میشه)
    }
    else if (boostChoice == 2 && isAttackCard)  // حمله
    {
        selectedCard->attack += selectedCard->boost;  // ← زیاد کردن حمله
        cout << "\n✅ Boost used for ATTACK!\n";
        cout << "   Attack increased by +" << selectedCard->boost << endl;
        cout << "   New attack value: " << selectedCard->attack << endl;
    }
    else if (boostChoice == 3 && isDefenseCard)  // دفاع
    {
        selectedCard->defense += selectedCard->boost;  // ← زیاد کردن دفاع
        cout << "\n✅ Boost used for DEFENSE!\n";
        cout << "   Defense increased by +" << selectedCard->boost << endl;
        cout << "   New defense value: " << selectedCard->defense << endl;
    }
    else
    {
        cout << "Invalid choice!\n";
    }
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

        if (attacker.getHero()->getName() == "Dracula")
        {
            DraculaWon = true;
            sherlockWon = false;
        }
        else if (attacker.getHero()->getName() == "Sherlock Holmes")
        {
            sherlockWon = true;
            DraculaWon = false;
        }


    }
    else if (attackValue < defenseValue) 
    {
        cout << "\n🛡️ " << defender.getName() << " blocks the attack!\n";

        if (defender.getHero()->getName() == "Dracula")
        {
            DraculaWon = true;
            sherlockWon = false;
        }
        else if (defender.getHero()->getName() == "Sherlock Holmes")
        {
            sherlockWon = true;
            DraculaWon = false;
        }
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

    boost(attackCard, true, false);

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

    boost(defenseCard, false, true);
    
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

        if (!cin.fail())
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
bool Controller::get_DraculaWon()
{
    return DraculaWon;
}
bool Controller::get_SherlockWon()
{
    return sherlockWon;
}

Controller::~Controller()
{

}
