#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include "core/Controller.hpp"

using namespace std;

Controller::Controller()
{
    DraculaWon = false;
    sherlockWon = false;
   // Guess = false;
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



void Controller::chooseCharacters()
{
    int cha;
	cout << "\nCharacters:" << endl;
    cout << " 1.Dracula" << endl;
    cout << " 2.Sherlock" << endl;

    cout << firstPlayer->getName() << ", choose your Character: ";
    cha = getChoice({1,2});

    firstPlayer->chooseCharacter(cha , 1);

    if (cha == 1) 
    {
        secondPlayer->chooseCharacter(2 , 2);  
    } 
    
    else 
    {
        secondPlayer->chooseCharacter(1 , 2);  
    }

    int pos;
    cout << firstPlayer->getName() << ", choose your Character position(4 or 15): ";
    pos = getChoice({4,15});

    bord.addCharacter( pos , firstPlayer->getHero() );
    bord.addCharacter( pos == 4 ? 15 : 4 , secondPlayer->getHero() );

    plaseSidekicks(*firstPlayer);
    plaseSidekicks(*secondPlayer);
}



void Controller::plaseSidekicks(Player& player)
{
    Character* hero = player.getHero();
    vector<int> zone = bord.getZoneCharacter(hero);
    vector<int> space  = bord.getEmptyZone(zone);
    vector<int> choose; 
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
                    choose.push_back(space[j]);
            }
            cout << "\nwhere do you want to place sister #" << i + 1 << ": ";
            s = getChoice(choose);
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
                choose.push_back(space[j]);
        }
        cout << "\nwhere do you want to place the Dr_watson: ";
        s = getChoice(choose);
        for(int j = 0 ; j < space.size()  ; j++)
        {
            if(!bord.getSpaceStatus(space[j]) && s == space[j])
                bord.addCharacter( s, player.getsidekick(1) );
        }

    }
}




void Controller::playTurn()
{
    int choose = 0;
    vector<Character*> choices;
    vector<int> valid;
    int Todo = 0;
    while(!end_game())
    {
        for(int i = 0; i < 2 ; i++)
        {
            cout << "\n════════════════════════════════════════════════════════════════" << endl;
            cout << "                  " << current->getName() << "'s turn\n";
    
            // نمایش TUI (نقشه و اطلاعات کاراتر ها و اطلاعات کارت ها)
    
            cout << "\nActions:  \n 1.Maneuver\n 2.Scheme\n 3.Attack";
            cout << "\nChoose a action: ";
            Todo = getChoice({1,2,3});
            
            Character* ch;
            int k = 1;
            
            switch(Todo)
            {
                case 1: //Maneuver
                {
                    cout << "\n═════════════════════════════════════════════════" << endl;
                    cout << "                 Maneuver\n"; 

                    firstPlayer->getDeck()->draw();
                    cout << "A Card added to your hand\n\n";

                    for (Character* ch : current->getCharacters())
                    {
                        if(ch->checkalive())
                        {
                            cout << k << "." << ch->getName() << endl;
                            choices.push_back(ch);
                            valid.push_back(k);
                            k++;
                        }
                    }
                    cout << "Choose a character to move: ";

                    choose = getChoice({valid});
                    Character* selected = choices[choose - 1];

                    move(selected);

                    break;
    
                }
                case 2: //Scheme
                {
                    cout << "\n═════════════════════════════════════════════════" << endl;
                    cout << "                 Scheme\n"; 

                    vector<int> choos;
                    choos = firstPlayer->getDeck()->showSchemeCards();

                    Card Schemecard;

                    int index;
                    cout << "\nChoose a card: ";
                    index = getChoice(choos);

                    Schemecard = firstPlayer->getDeck()->playCard(index - 1, Schemecard);

                    applyEffect(Schemecard);

                    break;
    
                }
                case 3: //Attack
                {
                    cout << "\n═════════════════════════════════════════════════" << endl;
                    cout << "                 Attack\n"; 

                    for (Character* ch : current->getCharacters())
                    {
                        if(ch->checkalive())
                        {
                            cout << k << "." << ch->getName() << endl;
                            valid.push_back(k);
                            k++;
                        }
                    }
                    cout << "Choose a character: ";
                    choose = getChoice({valid});
    
                    
                    startCombat(current , enemy);

                    break;
    

                }
                default:
                {
                    break;
                }
            }
    
            firstPlayer->getDeck()->draw();
            cout << "\nCard added to your hand\n";
             
            if(end_game()) 
            {
                break;
                
            }
        }

        //بیرون انداختن کارت اضافه
        swap(current, enemy);
    }

}



void Controller::move(Character* ch)
{
    int mov = ch->getMove();

    mov += boost();

    int place = ch->getSpace();
    
    vector<int> validSpaces;
    vector<int> currently;
    vector<int> next;
    vector<bool> visited(32,false);
    vector<int> tunnel;

    validSpaces.push_back(place);
    currently.push_back(place);
    visited[place] = true;


    while(mov--)
    {
    
        next.clear();

        for(int currentPos : currently)
        {
            vector<int> neighbors = bord.getAdjacentpos(currentPos);
            vector<int> tunnel = bord.getSecretPassages(currentPos);

            neighbors.insert(neighbors.end() , tunnel.begin() , tunnel.end());

            
            for(int pos : neighbors)
            {
                Character* target = bord.getCharacter(pos);


                if(visited[pos])
                {
                    continue;
                }
                else if(target == nullptr)
                {
                    validSpaces.push_back(pos);
                    next.push_back(pos);
    
                    visited[pos]=true;
                }
                else if(ch->getowner() == target->getowner())
                {
                    next.push_back(pos);
                    visited[pos]=true;
                }
                else if(ch->getowner() != target->getowner())
                {
                    continue;
                }
                
            }
        }
        currently = next;
    }

    cout << "\nAvailable spaces:   ";

    for(int pos : validSpaces)
    {
        cout << pos << "   ";
    }
    cout << "\nSelect a destination: ";

    int destination = getChoice(validSpaces);

    bord.addCharacter(destination, ch);
    bord.deletCharacter(place);
}



int Controller::boost()
{

    
    cout << "Do you want to use Boost? (y/n): ";
    char choice;
    cin >> choice;
    
    if (choice != 'y' && choice != 'Y')
    {
        cout << "No boost used.\n";
        return 0;
    }
    
    firstPlayer->getDeck()->showHand();

    int choos;
    choos = firstPlayer->getDeck()->gethandSize();

    cout << "Selected card: ";
    int select = getInt();
    for(int i = 0 ; select < 0 || select > choos ; i++)
    {
        cout << "Invalid choice\n";
        cout << "Selected card: ";
        select = getInt();
    }

    Card selectedCard;
    selectedCard = firstPlayer->getDeck()->playCard(select - 1, selectedCard);

    cout << "\n✅ Boost used for MOVEMENT!\n";
    cout << "   +" << selectedCard.getBoost() << " steps added.\n";

    return selectedCard.getBoost();

}


void Controller::startCombat(Player* attacker, Player* defender)
{
    cout << "\n" << attacker->getName() << " - Select ATTACK card:\n";
    attacker->getDeck()->showHand();
    
    int chooseDR;
    Card attackCard;
    while (true)
    {
        cout << "Enter card number (1-" << attacker->getDeck()->gethandSize() << "): ";
        chooseDR = getInt();

        attackCard = attacker->getDeck()->playCard(chooseDR - 1, attackCard);

        cout << "Invalid choice\n";
        if(attackCard.getName() == "")
        {
            cout << "Invalid choice\n";
        }
    }

    cout << "\n" << defender->getName() << " - Select DEFENSE card:\n";
    defender->getDeck()->showHand();
    
    int chooseSH;
    Card defenseCard;
    while (true)
    {
        cout << "Enter card number (1-" << defender->getDeck()->gethandSize() << "): ";
        chooseSH = getInt();

        defenseCard = defender->getDeck()->playCard(chooseSH - 1, defenseCard);
            
        if(defenseCard.getName() == "")
        {
            cout << "Invalid choice\n";
        }
    }

    resolveCombat(attackCard, defenseCard, attacker, defender);
    
    burncards.push_back(attackCard);
    burncards.push_back(defenseCard);
}


void Controller::resolveCombat(Card& attackCard, Card& defenseCard, Player* attacker, Player* defender) 
{
    
    int attackValue = attackCard.getAttack();;
    int defenseValue = attackCard.getAttack();;
    
    
    cout << "\n═══════════════════════════════════════════════════════\n";
    cout << "  ⚔️ RESOLVING COMBAT ⚔️\n";
    cout << "═══════════════════════════════════════════════════════\n";
    
    cout << "\n🔹 ATTACKER (" << attacker->getName() << "):\n";
    attacker->getDeck()->showCard(attackCard);
    
    cout << "=======================================================================";

    cout << "\n🔸 DEFENDER (" << defender->getName() << "):\n";
    defender->getDeck()->showCard(defenseCard);
    
    cout << "\n📊 COMBAT RESULT:\n";
    cout << "  ⚔️ Attack  : " << attackValue << "\n";
    cout << "  🛡️ Defense : " << defenseValue << "\n";
    
    if (attackValue > defenseValue) 
    {
        int damage = attackValue - defenseValue;
        cout << "\n💥 " << attacker->getName() << " deals " << damage << " damage!\n";
        defender->getHero()->takeDamage(defenseValue, attackValue);
        cout << "  " << defender->getName() << " HP: " << defender->getHero()->getHp() 
        << "/" << defender->getHero()->getMaxhp() << "\n";
    }
    else if (attackValue < defenseValue) 
    {
        cout << "\n🛡️ " << defender->getName() << " blocks the attack!\n";
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
        cout << "Name    : " << burncards[i].getName() << endl;
        cout << "Owner   : " << burncards[i].getOwnerString() << endl;
        cout << "Type    : " << burncards[i].getTypeString() << endl;
        cout << "Attack  : " << burncards[i].getAttack() << endl;
        cout << "Boost   : " << burncards[i].getBoost() << endl;
        cout << "Effect  : " << burncards[i].geteffect() << endl;
        cout << "--------------------------------------------------------------\n";
    }
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


void Controller::applyEffect(const Card& card)
{
    //current & enemy & bord are already defined

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Dracula

    
    if (card.getName() == "Bloodlust")
    {
        
    }
    
    else if (card.getName() == "Mist Form")
    {
        
    }
    
    else if (card.getName() == "Ambush")
    {
        
    }
    
    else if (card.getName() == "Blood Bath")
    {
        
    }
    
    else if (card.getName() == "Beast Form")
    {
        
    }
    
    else if (card.getName() == "Assault")
    {
        
    }
    
    else if (card.getName() == "Exploitation")
    {
        
    }
    
    else if (card.getName() == "Look Into My Eyes")
    {
        
    }
    
    else if (card.getName() == "Hunt")
    {
        
    }
    
    else if (card.getName() == "Insatiable Seduction")
    {
        
    }
    
    else if (card.getName() == "Thirst for Survival")
    {
        
    }
    
    else if (card.getName() == "Deception")
    {
        cout << "No special effect for this card.\n";
    }


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Sherlock


    if (card.getName() == "First Aid")
    {

        Character* holmes = current->getHero();
        Character* watson = current->getsidekick(1);
        vector<int> adjacent = bord.getAdjacentCharacter(holmes);
        vector<int> emptyAdjacent;


        for (auto pos : adjacent)
        {
            if (bord.isEmpty(pos))
            {
                emptyAdjacent.push_back(pos);
            }
            
        }
        if (!emptyAdjacent.empty())
        {;
            for (int i = 0; i < 32; i++)
            {
                if (bord.getCharacter(i) == watson)
                {
                    bord.deletCharacter(i);
                    break;
                }
            }
        }

        for (int i = 0; i <emptyAdjacent.size() ; i++)
        {
            if(bord.isEmpty(emptyAdjacent[i]))
            {
                bord.addCharacter(emptyAdjacent[i],watson);      
            }
            
        }
        
          
        holmes->heal(1);
        current->getDeck()->draw();
        cout << "Holmes healed 1 HP and 1 card drawn!\n";
        

    }
    
    else if (card.getName() == "Confirm Suspicion")
    {
        
        cout << "Enter a number for choose card ";
    }
    
    else if (card.getName() == "Counterattack")
    { 
        Character* holmes = current->getHero();
        Character* enemyHero = enemy->getHero();
        Character* enemySidekick = enemy->getsidekick(1);

        vector<int> adjacent = bord.getAdjacentCharacter(holmes);
        bool damaged = false;

        for (int pos : adjacent)
        {
            if (!bord.isEmpty(pos))
            {
                Character* target = bord.getCharacter(pos);
                
                if (target == enemyHero || target == enemySidekick)
                {
                    target->takeDamage(0, 2);
                    cout << "2 damage dealt to " << target->getName() << "!\n";
                    damaged = true;
                    break;  
                }
            }
        }
        
        if (!damaged)
        {
            cout << "No adjacent enemy fighter!\n";
        }
        

    }
    
    else if (card.getName() == "Strategic Deduction")
    {
        
    }
    
    else if (card.getName() == "Learning Never Ends")
    {
        
        if (get_DraculaWon())
        {
            cout << "Dracula won! Opponent draws 1 card.\n";
            enemy->getDeck()->draw();
        }
        else if (get_SherlockWon())
        {
            cout << "Sherlock won! Opponent draws 1 card.\n";
            enemy->getDeck()->draw();
        }
        else
        {
            cout << "You lost or tied! You draw 2 cards.\n";
            current->getDeck()->draw(2);
        }
        
    }
    
    else if (card.getName() == "Elementary")
    {
        /*
        cout << "Guess the attack value: ";
        int guess;
        cin >> guess;
        if (برای اون بولین هستت)
        {
            cout <<"You guessed -> succeessful";
        }
        else
        {
            cout <<"You guessed -> failed";

        }
        */
    }
    
    else if (card.getName() == "Impossible Elimination")
    {
        
    }
    
    else if (card.getName() == "Deception")
    {
        
    }
    
    else if (card.getName() == "A Fixed Point in a Changing Age")
    {
        
    }
    
    else if (card.getName() == "Master of Disguise")
    {
        
    }
    
    else if (card.getName() == "The Game Is Afoot")
    {
        
    }
    
    else if (card.getName() == "Sidearm")
    {
        
    }
    
    else if (card.getName() == "Study Methods")
    {
        
    }
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