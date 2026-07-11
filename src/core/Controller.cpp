#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <stdexcept>
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
    vector<int> zone = bord.getCharacterZone(hero);
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
                case 1:
                {
                    cout << "\n═════════════════════════════════════════════════" << endl;
                    cout << "                 Maneuver\n"; 

                    try
                    {
                        current->getDeck()->draw();
                        cout << "added to " << current->getName() << " hand\n\n";
                    }
                    catch(const runtime_error& e)
                    {
                        cout << e.what() << endl;
                        for(int i = 0 ; i <  current->getfighterCount() ; i++)
                        {
                            current->getsidekick(i)->takeDamage(2,0);
                        }
                        cout << "All character on team took 2 damage";
                    }

                    move();

                    break;
    
                }
                case 2:
                {

                    Scheme();

                    break;

                }
                case 3:
                {
                    
                    startCombat();

                    break;

                }
                default:
                {
                    break;
                }

                for(int i = 0 ; i < current->getfighterCount() ; i++)
                {
                    if(!current->getsidekick(i)->getIsAlive())
                    bord.deletCharacter(current->getHero()->getSpace());

                    if(!enemy->getsidekick(i)->getIsAlive())
                    bord.deletCharacter(enemy->getHero()->getSpace());
                }

            }
             
            if(end_game()) 
            break;
        }

        int selec;
        int HandSize;
        HandSize = current->getDeck()->gethandSize();

        for(int i = 0 ; HandSize > 7 ; i++)
        {
            current->getDeck()->showHand(current->getName());
            cout << endl << "Enter the card number to remove: ";
            selec = getInt();

            Card deletcadr;
            deletcadr = current->getDeck()->playCard(selec - 1 , deletcadr);
            for(int i = 0 ;deletcadr.getName() == ""; i++)
            {
                cout << "Invalid card number. Try again: ";
                selec = getInt();
                deletcadr = current->getDeck()->playCard(selec - 1 , deletcadr);
            }
            cout << endl << deletcadr.getName() <<  " was removed from your hand.\n";
        }


        swap(current, enemy);
    }

}



void Controller::move()
{
    int choose = 0;
    vector<Character*> choices;
    vector<int> valid;
    int number = 1;
    int m = 1;

    for (Character* ch : current->getCharacters())
    {
        if(ch->checkalive())
        {
            cout << number << "." << ch->getName() << endl;
            choices.push_back(ch);
            valid.push_back(m);
            number++;
        }
        m++;
    }
    cout << "Choose a character to move: ";
    choose = getChoice({valid});
    Character* selected = choices[choose - 1];


    int mov = selected->getMove();

    mov += boost();

    int place = selected->getSpace();
    
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
            vector<int> neighbors = bord.getposAdjacent(currentPos);
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
                else if(selected->getowner() == target->getowner())
                {
                    next.push_back(pos);
                    visited[pos]=true;
                }
                else if(selected->getowner() != target->getowner())
                {
                    visited[pos]=true;
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

    bord.addCharacter(destination, selected);
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
    
    current->getDeck()->showHand(current->getName());

    int choos;
    choos = current->getDeck()->gethandSize();

    cout << "Selected card: ";
    int select = getInt();
    for(int i = 0 ; select < 0 || select > choos ; i++)
    {
        cout << "Invalid choice\n";
        cout << "Selected card: ";
        select = getInt();
    }

    Card selectedCard;
    selectedCard = current->getDeck()->playCard(select - 1, selectedCard);

    cout << "\n✅ Boost used for MOVEMENT!\n";
    cout << "   +" << selectedCard.getBoost() << " steps added.\n";

    return selectedCard.getBoost();

}

void Controller::Scheme()
{
    cout << "\n═════════════════════════════════════════════════" << endl;
    cout << "                 Scheme\n"; 
    
    vector<int> choos;
    choos = current->getDeck()->showSchemeCards();
    if(choos.empty())
    {
        cout << "You don't have any Scheme cards.\n";
        return;
    }
    int index;
    while(true)
    {
        int choose = 0;
        vector<Character*> choices;
        vector<int> valid;
        int number = 1;
        int m = 1;
    
        for (Character* ch : current->getCharacters())
        {
            if(ch->checkalive())
            {
                cout << number << "." << ch->getName() << endl;
                choices.push_back(ch);
                valid.push_back(m);
                number++;
            }
            m++;
        }
        cout << "Choose a character: ";
        choose = getChoice({valid});
        Character* selected = choices[choose - 1];

        cout << "\nChoose a card: ";
        index = getChoice(choos);
        
        bool ownerOK = false;

        const vector<Card>& hand = current->getDeck()->gethand();

        Card card = hand[index - 1];

        if(selected->isHero())
        {
            ownerOK = card.isHero() || card.isAnyowner();
        }
        else
        {
            ownerOK = card.issideKick() || card.isAnyowner();
        }
    
        if(ownerOK)
        {
            break;
        }
        cout << "This fighter can't use this card.\n";
    }

    Card Schemecard;
    Schemecard = current->getDeck()->playCard(index - 1, Schemecard);

    applyEffect(Schemecard);

    cout << "\nScheme effect applied.\n";
}



void Controller::startCombat()
{
    int choose = 0;
    vector<Character*> choices;
    vector<int> valid;
    int number = 1;
    int m = 1;

    cout << "\n═════════════════════════════════════════════════" << endl;
    cout << "                 Attack\n"; 

    for (Character* ch : current->getCharacters())
    {
        if(ch->checkalive() && bord.canAttack(ch->getAttacktype() , ch->getSpace()))
        {
            cout << number << "." << ch->getName() << endl;
            choices.push_back(ch);
            valid.push_back(m);
            number++;
        }
        m++;
    }
    cout << "Choose a character to attack with: ";
    choose = getChoice({valid});
    Character* attacker = choices[choose - 1];


    valid.clear();
    number = 1;
    m = 1;
    

    vector<Character*> ch = bord.getAttackCharacters(attacker->getAttacktype() , attacker->getSpace());

    for(int i = 0 ; i < ch.size() ; i++)
    {
        if(ch[i]->checkalive() && attacker->getowner() != ch[i]->getowner())
        {
            cout << number << "." << ch[i]->getName() << endl;
            choices.push_back(ch[i]);
            valid.push_back(m);
            number++;
        }
        m++;
    }
    if(number != 1)
    {
        cout << "Choose a character to attack: ";
        choose = getChoice({valid});
        Character* defender = choices[choose - 1];
    
        Card attackCard = chooseCombatCard(current , attacker , true);
        Card defenseCard = chooseCombatCard(enemy , defender , false);
    
        resolveCombat(attackCard, defenseCard);
    }
    else
    {
        cout << "\nThere are no enemies you can attack.\n";
    }
}

Card Controller::chooseCombatCard(Player* player , Character* fighter, bool attack)
{
    cout << "\nYour hand:\n";
    player->getDeck()->showHand(current->getName());
    while (true)
    {

        cout << "\nChoose a card: ";
        int choice;
        choice = getInt();
        choice--;

        const vector<Card>& hand = player->getDeck()->gethand();

        if(choice < 0 || choice >= hand.size())
        {
            cout << "Invalid number.\n";
            continue;
        }

        Card card = hand[choice];


        bool ownerOK = false;

        if(fighter->isHero())
        {
            ownerOK = card.isHero() || card.isAnyowner();
        }
        else
        {
            ownerOK = card.issideKick() || card.isAnyowner();
        }

        if(!ownerOK)
        {
            cout << "This fighter can't use this card.\n";
            continue;
        }


        bool typeOK = false;

        if(attack)
        {
            typeOK = card.isAttack() || card.isVersatile();
        }
        else
        {
            typeOK = card.isDefense() || card.isVersatile();
        }

        if(!typeOK)
        {
            cout << "Wrong card type.\n";
            continue;
        }

        Card attakingcard;
        return player->getDeck()->playCard(choice , attakingcard);
    }
}


void Controller::resolveCombat(Card& attackCard, Card& defenseCard) 
{
    
    int attackValue = attackCard.getAttack();;
    int defenseValue = attackCard.getAttack();;
    
    
    cout << "\n═══════════════════════════════════════════════════════\n";
    cout << "  ⚔️ RESOLVING COMBAT ⚔️\n";
    cout << "═══════════════════════════════════════════════════════\n";
    
    cout << "\n🔹 ATTACKER (" << current->getName() << "):\n";
    current->getDeck()->showCard(attackCard);
    
    cout << "=======================================================================";

    cout << "\n🔸 DEFENDER (" << enemy->getName() << "):\n";
    enemy->getDeck()->showCard(defenseCard);
    
    
    if(defenseCard.isBeforeCombat())
    {
        applyEffect(defenseCard);
    }
    if(attackCard.isBeforeCombat())
    {
        applyEffect(attackCard);
    }

    cout << "\n📊 COMBAT RESULT:\n";
    cout << "  ⚔️ Attack  : " << attackValue << "\n";
    cout << "  🛡️ Defense : " << defenseValue << "\n";
    
    
    if(defenseCard.isDuringCombat())
    {
        applyEffect(defenseCard);
    }
    if(attackCard.isDuringCombat())
    {
        applyEffect(attackCard);
    }
    

    if (attackValue > defenseValue) 
    {
        int damage = attackValue - defenseValue;
        cout << "\n💥 " << current->getName() << " deals " << damage << " damage!\n";
        enemy->getHero()->takeDamage(defenseValue, attackValue);

        if(!enemy->getHero()->getIsAlive())
        bord.deletCharacter(enemy->getHero()->getSpace());

        cout << "  " << enemy->getName() << " HP: " << enemy->getHero()->getHp() 
        << "/" << enemy->getHero()->getMaxhp() << "\n";
    }
    else if (attackValue <= defenseValue) 
    {
        cout << "\n🛡️ " << enemy->getName() << " blocks the attack!\n";
    }


    
    if(defenseCard.isAfterCombat())
    {
        applyEffect(defenseCard);
    }
    if(attackCard.isAfterCombat())
    {
        applyEffect(attackCard);
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
        vector<int> adjacent = bord.getCharacterAdjacent(holmes);
        vector<int> emptyAdjacent;


        for (auto pos : adjacent)
        {
            if (bord.isEmpty(pos))
            {
                emptyAdjacent.push_back(pos);
            }
            
        }
        if (!emptyAdjacent.empty())
        {
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
        
    }
    
    else if (card.getName() == "Counterattack")
    { 
        Character* holmes = current->getHero();
        Character* enemyHero = enemy->getHero();
        Character* enemySidekick = enemy->getsidekick(1);

        vector<int> adjacent = bord.getCharacterAdjacent(holmes);
        bool damaged = false;

        for (int pos : adjacent)
        {
            if (!bord.isEmpty(pos))
            {
                Character* target = bord.getCharacter(pos);
                
                if (target == enemyHero || target == enemySidekick)
                {
                    target->takeDamage(0, 2);

                    if(!target->getIsAlive())
                    bord.deletCharacter(target->getSpace());

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
        Character* holmes = current->getHero();
        Character* enemyHero = enemy->getHero();
        
        cout << "Guess the attack value: ";
        int guess;
        cin >> guess;
        if ()/*برای اون بولین هستت*/
        {
            cout <<"You guessed -> succeessful";
        }
        else
        {
            cout <<"You guessed -> failed";

        }
        
    }
    
    else if (card.getName() == "Impossible Elimination")
    {
        
    }
    
    else if (card.getName() == "Deception")
    {
        
    }
    
    else if (card.getName() == "A Fixed Point in a Changing Age")
    {
    Character* holmes = current->getHero();
    Character* watson = current->getsidekick(1);

    vector<int> adjacent = bord.getCharacterAdjacent(holmes);

    bool found = false;

    for (int pos : adjacent)
    {
        if (bord.getCharacter(pos) == watson)
        {
            holmes->heal(1);
            watson->heal(1);

            cout << "Holmes and Watson each heal 1 HP.\n";
            found = true;
            break;
        }
    }

    if (!found)
    {
        cout << "Watson is not adjacent to Holmes.\n";
    }

    }
    
    else if (card.getName() == "Master of Disguise")
    {
        
    }
    
    else if (card.getName() == "The Game Is Afoot")
    {
        
    }
    
    else if (card.getName() == "Sidearm")
    {

        cout << "no effect (-_-)";
    }
    
    else if (card.getName() == "Study Methods")
    {
    if (get_SherlockWon())
    {
        cout << "Sherlock won the combat.\n";
        cout << "Opponent's hand:\n";
        enemy->getDeck()->showHand("Dracula");
    }
    else
    {
        cout << "Card effect not activated.\n";
    }
        
    }
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