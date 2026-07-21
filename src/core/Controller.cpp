#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <random>
#include "core/Controller.hpp"
#include "GameTUI.hpp"

using namespace std;

void Controller::choosePlayers(Player player[2])
{
    string n;
    int a;

    cout << "\nFirst player, enter your name: ";
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
        current = &player[0];
        enemy = &player[1];
    }
    else
    {
        current = &player[1];
        enemy = &player[0];
    }
}



void Controller::chooseCharacters()
{
    int cha;
    cout << "\nCharacters:" << endl;
    cout << " 1.Dracula" << endl;
    cout << " 2.Sherlock" << endl;

    cout << current->getName() << ", choose your Character: ";
    cha = getChoice({1,2});

    current->chooseCharacter(cha , 1);

    if (cha == 1) 
    {
        enemy->chooseCharacter(2 , 2);  
    } 
    else 
    {
        enemy->chooseCharacter(1 , 2);  
    }

    int pos;
    cout << current->getName() << ", choose your Character position (1 = left map ) (2 = righ map): ";
    pos = getChoice({1,2});

    if(pos == 1)
    {
        bord.addCharacter( 4 , current->getHero() );
        bord.addCharacter( 15 , enemy->getHero() );
    }
    else
    {
        bord.addCharacter( 15 , current->getHero() );
        bord.addCharacter( 4 , enemy->getHero() );
    }

    plaseSidekicks(*current);
    plaseSidekicks(*enemy);
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
        cout << "\n════════════════════════════════════════════════════════════════════" << endl;
        for(int i = 0 ; i < 3 ; i++)
        {
            cout << "Available space: ";
            for(int j = 0 ; j < space.size()  ; j++)
            {
                if(!bord.getSpaceStatus(space[j]))
                {
                    cout << "   " << space[j];  
                    choose.push_back(space[j]);
                }
            }
            cout << "\nwhere do you want to place sister #" << i + 1 << ": ";
            s = getChoice(choose);
            for(int j = 0 ; j < space.size()  ; j++)
            {
                if(!bord.getSpaceStatus(space[j]) && s == space[j])
                bord.addCharacter( s, player.getsidekick(i+1));
            }
            choose.clear();
        }
    }
    else
    {
        cout << "\n══════════════════════════════════════════════════════════════════" << endl;
        cout << "Available space: ";
        for(int j = 0 ; j < space.size()  ; j++)
        {
            if(!bord.getSpaceStatus(space[j]))
            {
                cout << "   " << space[j];  
                choose.push_back(space[j]);
            }
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
    int Todo = 0;
    while(!end_game())
    {
        
        gamerand = 0;
        while(gamerand < 2)
        {
            // نمایش TUI (نقشه و اطلاعات کاراتر ها و اطلاعات کارت ها)
            GameTUI::render(*current, *enemy, current, bord);

            cout << "\n═══════════════════════════════════════════════════════════════════════════════" << endl;
            cout << "                          " << current->getName() << "'s turn\n";
           
            if(current->getHero()->getName() == "Dracula")
            current->getHero()->ability(bord , current);

    
            cout << "\nActions:  \n 1.Maneuver\n 2.Scheme\n 3.Attack\n 4.End Turn";
            cout << "\nChoose a action: ";
            Todo = getChoice({1,2,3,4});
            
            Character* ch;
            int k = 1;
            
            switch(Todo)
            {
                case 1:
                {
                    cout << "\n══════════════════════════════════════════════════════════════════" << endl;
                    cout << "                          Maneuver\n"; 

                    try
                    {
                        current->getDeck()->draw();
                        cout << "✅1 card added to " << current->getName() << " hand\n\n";
                    }
                    catch(const runtime_error& e)
                    {
                        cout << e.what() << endl;
                        for(int i = 0 ; i <  current->getfighterCount() ; i++)
                        {
                            current->getsidekick(i)->takeDamage(2);
                        }
                        cout << "All character on team took 2 damage";
                    }

                    int mov = 0;
                    mov += boost();

                    int choose = 0;
                    vector<Character*> choices;
                    vector<int> valid;
                    int number = 1;

                    cout << "Your Characters:\n";
                    for (Character* ch : current->getCharacters())
                    {
                        if(ch->checkalive())
                        {
                            cout << number << "." << ch->getName() << endl;
                            choices.push_back(ch);
                            valid.push_back(number);
                            number++;
                        }
                    }
                    cout << "Choose a character to move: ";
                    choose = getChoice({valid});
                    Character* selected = choices[choose - 1];

                    mov += selected->getMove();

                    move(mov , selected);

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
                case 4:
                {
                    break;
                }
                default:
                {
                    break;
                }

                
            }
            for(int i = 0 ; i < current->getfighterCount() ; i++)
            {
                if(!current->getsidekick(i)->checkalive())
                bord.deletCharacter(current->getsidekick(i)->getSpace());

                if(!enemy->getsidekick(i)->checkalive())
                bord.deletCharacter(enemy->getsidekick(i)->getSpace());
            }
            
            gamerand++;

            if(end_game()) 
            break;
        }

        int index;
        int HandSize;
        HandSize = current->getDeck()->gethandSize();
        while(HandSize > 7)
        {
            current->getDeck()->showHand(current->getName());
            cout << endl << "Enter the card number to remove: ";
            index = getInt();

            while(index <= 0 || index > HandSize)
            {
                cout << "Invalid card number. Try again: ";
                index = getInt();
            }

            Card deletcadr;
            deletcadr = current->getDeck()->playCard(index - 1 , deletcadr);
            cout << endl << deletcadr.getName() << " was removed from your hand.\n";

            HandSize = current->getDeck()->gethandSize();
        }


        swap(current, enemy);
    }

}



void Controller::move(int mov ,Character* selected)
{
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
                    continue;
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
        cout << pos << "   ";
    cout << "\nSelect a destination: ";
    int destination = getChoice(validSpaces);

    bord.deletCharacter(place);
    bord.addCharacter(destination, selected);
}



int Controller::boost()
{
    cout << "Do you want to use Boost? (y/n): ";
    char choice;
    cin >> choice;
    
    if (choice != 'y' && choice != 'Y')
    {
        cout << "🟥No boost used.\n\n";
        return 0;
    }
    current->getDeck()->showHand(current->getName());
    int choos = current->getDeck()->gethandSize();
    cout << "Selected card: ";
    int select = getInt();
    for(int i = 0 ; select < 1 || select > choos + 1 ; i++)
    {
        cout << "Invalid choice\n";
        cout << "Selected card: ";
        select = getInt();
    }

    Card selectedCard;
    selectedCard = current->getDeck()->playCard(select - 1, selectedCard);

    cout << "\n🟦 Boost used for MOVEMENT!\n\n";
    cout << "   +" << selectedCard.getBoost() << " steps added.\n";

    return selectedCard.getBoost();
}

void Controller::Scheme()
{
    cout << "\n══════════════════════════════════════════════════════════════════════════" << endl;
    cout << "                                Scheme\n"; 
    
    vector<int> choos = current->getDeck()->getSchemeCardIndices();
    if(choos.empty())
    {
        cout << "You don't have any Scheme cards.\n";
        gamerand++;

        return;
    }

    Character* selected;
    int index;
    while(true)
    {
        int choose = 0;
        vector<Character*> choices;
        vector<int> valid;
        int number = 1;

        cout << "\nYour Characters:\n";
        for (Character* ch : current->getCharacters())
        {
            if(ch->checkalive())
            {
                cout << number << "." << ch->getName() << endl;
                choices.push_back(ch);
                valid.push_back(number);
                number++;
            }
        }
        cout << "Choose a character: ";
        choose = getChoice({valid});
        selected = choices[choose - 1];

        cout << "\nChoose a card: ";
        index = getChoice(choos);
        
        bool ownerOK = false;
        const vector<Card>& hand = current->getDeck()->gethand();
        Card card = hand[index - 1];

        if(selected->isHero())
            ownerOK = card.isHero() || card.isAnyowner();
        else
            ownerOK = card.issideKick() || card.isAnyowner();
    
        if(ownerOK)
            break;
        cout << "This fighter can't use this card.\n";
    }

    Card Schemecard;
    Schemecard = current->getDeck()->playCard(index - 1, Schemecard );

    applyEffectScheme(Schemecard , current , enemy , selected);

    cout << "\nScheme effect applied.\n";
}



void Controller::startCombat()
{
    cancelEffectDR = false;
    cancelEffectSH = false;
    GuessElementary = false;

    if(current->getDeck()->getAttackCardIndices().empty())
    {
        cout << "You have no attack cards. Cannot attack this turn.\n";
        gamerand++;
        return;
    }

    int choose = 0;
    vector<Character*> choices;
    vector<int> valid;
    int number = 1;

    cout << "\n══════════════════════════════════════════════════════════════════════════" << endl;
    cout << "                                Attack\n"; 

    for (Character* ch : current->getCharacters())
    {
        if(ch->checkalive() && bord.canAttack(ch->getAttacktype() , ch->getSpace()))
        {
            cout << number << "." << ch->getName() << endl;
            choices.push_back(ch);
            valid.push_back(number);
            number++;
        }
    }
    if(number == 1)
    {
        cout << "No targets are available to attack.\n";
        return;
    }
    cout << "Choose a character to attack with: ";
    choose = getChoice({valid});
    Character* attacker = choices[choose - 1];

    valid.clear();
    number = 1;
    vector<Character*> ch = bord.getAttackCharacters(attacker->getAttacktype() , attacker->getSpace());

    cout << "\nYour Characters:\n";
    for(int i = 0 ; i < ch.size() ; i++)
    {
        if(ch[i]->checkalive() && attacker->getowner() != ch[i]->getowner())
        {
            cout << number << "." << ch[i]->getName() << endl;
            choices.push_back(ch[i]);
            valid.push_back(number);
            number++;
        }
    }
    if(number != 1)
    {
        cout << "Choose a character to attack: ";
        choose = getChoice({valid});
        Character* defender = choices[choose - 1];

        Card attackCard = chooseCombatCard(current , attacker , true);
        Card defenseCard = chooseCombatCard(enemy , defender , false);
    
        resolveCombat(attackCard, defenseCard , attacker , defender);
    }
    else
    {
        cout << "\nThere are no enemies you can attack.\n";
    }
} 

Card Controller::chooseCombatCard(Player* player , Character* fighter, bool attack)
{
    vector<int> myhandcard;
    if(attack)
    {
        myhandcard = player->getDeck()->getAttackCardIndices();
        
    }
    else
    {
        myhandcard = player->getDeck()->getDefenseCardIndices();
    }
    
    if(myhandcard.empty())
    {
        cout << player->getName() << " has no valid " 
             << (attack ? "attack" : "defense") << " card. Using 0 value.\n";

        Card empty;
        return empty;
    }

    while (true)
    {
        cout << "\nChoose a card: ";

        cout << "Available cards: ";
        for(int idx : myhandcard) 
        cout << idx + 1 << "  ";

        cout << "\n> ";

        int choice;
        choice = getChoice(myhandcard);
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
            ownerOK = card.isHero() || card.isAnyowner();
        else
            ownerOK = card.issideKick() || card.isAnyowner();

        if(!ownerOK)
        {
            cout << "This fighter can't use this card.\n";
            continue;
        }

        bool typeOK = false;
        if(attack)
            typeOK = card.isAttack() || card.isVersatile();
        else
            typeOK = card.isDefense() || card.isVersatile();

        if(!typeOK)
        {
            cout << "Wrong card type.\n";
            continue;
        }

        Card attakingcard;
        return player->getDeck()->playCard(choice , attakingcard);
    }
}

void Controller::resolveCombat(Card& attackCard, Card& defenseCard , Character* attacker , Character* defender) 
{
    if(defenseCard.getName() == "Elementary")
    {
        cout << enemy->getName() << ", guess the attack value of your opponent's card.\n";
        int number = getInt();
        if(number == attackCard.getAttack())
        {
            GuessElementary = true;
        }
    }
    
    int attackValue = attackCard.getAttack();
    int defenseValue = defenseCard.getAttack();

    cout << "\n═══════════════════════════════════════════════════════════════════\n";
    cout << "                 ⚔️ RESOLVING COMBAT ⚔️\n";
    cout << "═════════════════════════════════════════════════════════════════════\n";
    
    if(defenseCard.isBeforeCombat())
        applyEffect(defenseCard , attackCard , enemy , current , attacker , defender , false);
    if(attackCard.isBeforeCombat())
        applyEffect(attackCard , defenseCard , current , enemy , attacker , defender , false);

    cout << "\n📊 COMBAT RESULT:\n";
    cout << "  ⚔️ Attack  : " << attackValue << "\n";
    cout << "  🛡️ Defense : " << defenseValue << "\n";
    
    if(defenseCard.isDuringCombat())
        applyEffect(defenseCard , attackCard , enemy , current , attacker , defender , false);
    if(attackCard.isDuringCombat())
        applyEffect(attackCard , defenseCard , current , enemy , attacker , defender , false);
    
    bool attackerWon = false;
    bool defenderWon = false;

    if (attackValue > defenseValue) 
    {
        int damage = attackValue - defenseValue;
        cout << "\n💥 " << current->getName() << " deals " << damage << " damage!\n";
        enemy->getHero()->takeDamage(damage);

        if(!enemy->getHero()->checkalive())
            bord.deletCharacter(enemy->getHero()->getSpace());

        cout << "  " << enemy->getName() << " HP: " << enemy->getHero()->getHp() 
        << "/" << enemy->getHero()->getMaxhp() << "\n";
        attackerWon = true;
    }
    else 
    {
        cout << "\n🛡️ " << enemy->getName() << " blocks the attack!\n";
        defenderWon = true;
    }

    if(defenseCard.isAfterCombat())
        applyEffect(defenseCard , attackCard , enemy , current , attacker , defender , defenderWon);
  
    if(attackCard.isAfterCombat())
        applyEffect(attackCard , defenseCard , current , enemy , attacker , defender , attackerWon);
  
    cancelEffectDR = false;
    cancelEffectSH = false;
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

int Controller::getChoice(std::vector<int> valid)
{
    while(true)
    {
        int x = getInt();
        for(int i : valid)
            if(i == x) return x;
        std::cout << "Invalid choice. Try again: ";
    }
}


Bord& Controller::getBord()
{
    return bord;
}

Player* Controller::getCurrentPlayer()
{
    return current;
}

Player* Controller::getEnemyPlayer()
{
    return enemy;
}

Character* Controller::getCharacterAt(int position)
{
    return bord.getCharacter(position);
}

bool Controller::isGameOver()
{
    return end_game();
}


bool Controller::end_game() const
{
    Character* hero1 = current->getHero();
    Character* hero2 = enemy->getHero();

    if (!hero1->checkalive() || !hero2->checkalive())
    {
        cout << "\n══════════════════════════════════════════════════════════════════════════\n";
        cout << "                                GAME OVER\n";
        cout << "══════════════════════════════════════════════════════════════════════════\n";
        if (!hero1->checkalive())
            cout << "           🏆 " << hero2->getName() << " wins! 🏆\n";
        else
            cout << "           🏆 " << hero1->getName() << " wins! 🏆\n";
  
        cout << "══════════════════════════════════════════════════════════════════════════\n";
        return true;
    }
    return false;
}