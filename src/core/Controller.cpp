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
    cout << current->getName() << ", choose your Character position(4 or 15): ";
    pos = getChoice({4,15});

    bord.addCharacter( pos , current->getHero() );
    bord.addCharacter( pos == 4 ? 15 : 4 , enemy->getHero() );

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
    int Todo = 0;
    while(!end_game())
    {
        gamerand = 0;
        while(gamerand < 2)
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

                    int mov = 0;
                    mov += boost();

                    int choose = 0;
                    vector<Character*> choices;
                    vector<int> valid;
                    int number = 1;

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

                    mov =+ selected->getMove();

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
            
            gamerand++;

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
    Character* selected;
    int index;
    while(true)
    {
        int choose = 0;
        vector<Character*> choices;
        vector<int> valid;
        int number = 1;
    
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
    Schemecard = current->getDeck()->playCard(index - 1, Schemecard );

    applyEffect(Schemecard , Schemecard , current , enemy , selected , selected);

    cout << "\nScheme effect applied.\n";
}



void Controller::startCombat()
{
    int choose = 0;
    vector<Character*> choices;
    vector<int> valid;
    int number = 1;

    cout << "\n═════════════════════════════════════════════════" << endl;
    cout << "                 Attack\n"; 

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
    cout << "Choose a character to attack with: ";
    choose = getChoice({valid});
    Character* attacker = choices[choose - 1];


    valid.clear();
    number = 1;
    

    vector<Character*> ch = bord.getAttackCharacters(attacker->getAttacktype() , attacker->getSpace());

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


void Controller::resolveCombat(Card& attackCard, Card& defenseCard , Character* attacker , Character* defender) 
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
        applyEffect(defenseCard , attackCard , enemy , current , attacker , defender);
    } 
    if(attackCard.isBeforeCombat())
    {
        applyEffect(attackCard , defenseCard , current , enemy , attacker , defender);
    }

    cout << "\n📊 COMBAT RESULT:\n";
    cout << "  ⚔️ Attack  : " << attackValue << "\n";
    cout << "  🛡️ Defense : " << defenseValue << "\n";
    
    
    if(defenseCard.isDuringCombat())
    {
        applyEffect(defenseCard , attackCard , enemy , current , attacker , defender);
    }
    if(attackCard.isDuringCombat())
    {
        applyEffect(attackCard , defenseCard , current , enemy , attacker , defender);
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
        applyEffect(defenseCard , attackCard , enemy , current , attacker , defender);
    }
    if(attackCard.isAfterCombat())
    {
        applyEffect(attackCard , defenseCard , current , enemy , attacker , defender);
    }

    FeintDR = false;
    FeintSH = false;
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
    Character* hero1 = current->getHero();
    Character* hero2 = enemy->getHero();

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



void Controller::applyEffect(Card& card , Card& enemycard ,Player* self, Player* opponent , Character* attacker , Character* defender)
{
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Dracula

    if (card.getName() == "Feeding Frenzy")
    {
        cout << card.geteffect() << endl;//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  for ALL

        vector<int> defendzon = bord.getCharacterZone(defender);

        int amount = 0;

        for(int adjen : defendzon)
        {
            if(bord.getSpaceStatus(adjen))
            {
                for(int i = 1 ; i < 4 ; i++)
                {
                    if(bord.getCharacter(adjen) == bord.getCharacter(self->getsidekick(i)->getSpace()))
                    {
                        amount++;
                    }
                }
            }
        }
        cout << "Card " << card.getName() << "'s attack increased by " << amount << ".\n";
        card.setAttack(card.getAttack() + amount);
        return;//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  for ALL
    }
    
    else if (card.getName() == "MistForm")
    {
        cout << card.geteffect() << endl;

        int pos;
        while(true)
        {
            cout << "Choose a space for Dracula: ";
            pos = getInt();
            if(pos > 0 || pos < 31 || bord.isEmpty(pos))
            {
                bord.deletCharacter(self->getHero()->getSpace());
                bord.addCharacter(pos , self->getHero());
                break;
            }
            cout << "Invaliad input. Plase try again." << endl;
        }
        cout << "Dracula was placed\n" ;
        cout << "You have gained an extra action.\n";

        gamerand--;
        return;
    }
    
    else if (card.getName() == "Ambush")
    {
        cout << card.geteffect() << endl;

        int random = rand() % opponent->getDeck()->gethandSize();
        
        Card boostCard;
        boostCard = opponent->getDeck()->playCard(random , boostCard);

        cout << "Card " << boostCard.getName() << " was removed from " << opponent->getName() << "'s hand.\n";

        int boost = boostCard.getBoost();
        
        card.setAttack(boost + card.getAttack());

        cout << boostCard.getBoost() << " boost was added to " << card.getName() << "'s attack.\n";
        return;
    }
    
    else if (card.getName() == "Baptism of Blood")
    {
        cout << card.geteffect() << endl;

        self->getHero()->heal(2);
        cout << "Dracula recovered 2 health.\n";

        vector<int> zonCanPlace;
        zonCanPlace = bord.getCharacterZone(self->getHero());
        
        int pos;
        for(int i = 1 ; i < 4 ; i++)
        {
            zonCanPlace = bord.getEmptyZone(zonCanPlace);
            if(!self->getsidekick(i)->checkalive())
            {
                cout << "\nAvailable spaces for the Sister:  ";
                for(int i = 0 ; i < zonCanPlace.size() ; i++)
                {
                    cout << zonCanPlace[i] << "   ";
                }
                pos = getChoice(zonCanPlace);
                self->getsidekick(i)->setSpace(pos);
                self->getsidekick(i)->heal(1);
                bord.addCharacter(pos , self->getsidekick(i));
                cout << self->getsidekick(i)->getName() << " place on " << pos;

            }
        }
        return;
    }
    
    else if (card.getName() == "BeastForm")
    {
        cout << card.geteffect() << endl;
        while(true)
        {
            cout << "Do you want to remove a card?(y/n): ";
            char option;
            cin >> option;
            if(option == 'y' || option == 'Y')
            {
                self->getDeck()->showHand(self->getName());
                int handSize = self->getDeck()->gethandSize();
    
                int choice;
                while (true)
                {
                    cout << "choice a card: ";
                    choice = getInt();
                    if(choice > 0 ; choice < handSize)
                    {
                        break;
                    }
                    cout << "Invalid input.";
                }

                Card selected;
                selected = self->getDeck()->playCard(choice , selected);
    
                card.setAttack(card.getAttack() + 1);
                cout << "Card " << card.getName() << " gained +1 attack.\n";
            }
            else
            {
                break;
            }
        }
        return;
    }
    
    else if (card.getName() == "Dash")
    {
        cout << card.geteffect() << endl;
        move(3 , attacker);
        return;
    }
    
    else if (card.getName() == "Exploit")
    {
        cout << card.geteffect() << endl;

        try
        {
            self->getDeck()->draw();
            cout << "added to " << self->getName() << " hand\n\n";
        }
        catch(const runtime_error& e)
        {
            cout << e.what() << endl;
            for(int i = 0 ; i <  self->getfighterCount() ; i++)
            {
                self->getsidekick(i)->takeDamage(2,0);
            }
            cout << "All character on team took 2 damage";
        }
        return;
    }
    
    else if (card.getName() == "Look Into My Eyes")
    {
        cout << card.geteffect() << endl;

        int enemyboost = enemycard.getBoost();

        card.setAttack(enemyboost);

        cout << "This card gained " << enemyboost << " defense.\n";
        return;
    }
    
    else if (card.getName() == "Prey Upon")
    {
        cout << card.geteffect() << endl;

        int amount = 0;
        vector<int> draculaAdjence = bord.getCharacterAdjacent(attacker);
        for(int adjen : draculaAdjence)
        {
            if(bord.getCharacter(adjen)->getowner() != attacker->getowner())
            {
                bord.getCharacter(adjen)->takeDamage(0 , 1);
                cout << bord.getCharacter(adjen)->getName() << "took 1 damage\n";
                amount++;
            }
        }
        attacker->heal(amount);
        cout << attacker->getName() << " gained " << amount << " health.\n";
        return;
    }
    
    else if (card.getName() == "Ravening Seduction")
    {
        cout << card.geteffect() << endl;

        int choose = 0;
        vector<Character*> choices;
        vector<int> valid;
        int number = 1;
    
        for (Character* ch : self->getCharacters())
        {
            if(ch->checkalive())
            {
                cout << number << "." << ch->getName() << endl;
                choices.push_back(ch);
                valid.push_back(number);
                number++;
            }
        }
        for (Character* ch : opponent->getCharacters())
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

        move(2 , selected);

        vector<int> target = bord.getCharacterAdjacent(selected);
        number = 0;
        for(int adjenc : target)
        {
            if(!bord.isEmpty(adjenc))
            {
                for(int i = 1 ; i < 4 ; i++)
                {
                    if(self->getsidekick(i)->getIsAlive())
                    {
                        if(bord.getCharacter(adjenc) == self->getsidekick(i))
                        {
                            number++;
                        }
                    }
                }
            }
        }

        selected->takeDamage(0 , number);
        if(number > 0)
        {
            cout << selected->getName() << "took " << number << " damage." << endl;
        }
        else
        {
            cout << selected->getName() << "took no damage." << endl;
        }
        return;
    }
    
    else if (card.getName() == "Thirst for Sustenance")//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    {
        cout << card.geteffect() << endl;
        // if()
        // {
        // }
            int place = defender->getSpace();
    
            vector<int> validSpaces = bord.getEmptyAdjacent(defender);

            cout << "\nAvailable spaces:   ";

            for(int pos : validSpaces)
            {
                cout << pos << "   ";
            }
            cout << "\nSelect a destination: ";

            int destination = getChoice(validSpaces);

            bord.deletCharacter(place);
            bord.addCharacter(destination, defender);

        
        return;
    }
    
    else if (card.getName() == "Feint")
    {
        cout << card.geteffect() << endl; 

        FeintDR = true;
    
        return;
    }


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Sherlock


    if (card.getName() == "Administer Aid")
    {

        Character* holmes = self->getHero();
        Character* watson = self->getsidekick(1);
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
        try
        {
            self->getDeck()->draw();
            cout << "added to " << self->getName() << " hand\n\n";
        }
        catch(const runtime_error& e)
        {
            cout << e.what() << endl;
            for(int i = 0 ; i <  self->getfighterCount() ; i++)
            {
                self->getsidekick(i)->takeDamage(2,0);
            }
            cout << "All character on team took 2 damage";
        }
        cout << "Holmes healed 1 HP and 1 card drawn!\n";
        

    }
    
    else if (card.getName() == "Confirm Suspicion")
    {
        
        cout << "Enter a number for choose card ";
    }
    
    else if (card.getName() == "Counterpunch")
    { 
        Character* holmes = self->getHero();
        Character* enemyHero = opponent->getHero();
        Character* enemySidekick = opponent->getsidekick(1);

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
    
    else if (card.getName() == "Deduce Strategy")
    {
        
    }
    
    else if (card.getName() == "Education Never Ends")
    {
        
        if (get_DraculaWon())
        {
            cout << "Dracula won! Opponent draws 1 card.\n";
            try
            {
                opponent->getDeck()->draw();
                cout << "added to " << opponent->getName() << " hand\n\n";
            }
            catch(const runtime_error& e)
            {
                cout << e.what() << endl;
                for(int i = 0 ; i <  opponent->getfighterCount() ; i++)
                {
                    opponent->getsidekick(i)->takeDamage(2,0);
                }
                cout << "All character on team took 2 damage";
            }
        }
        else if (get_SherlockWon())
        {
            cout << "Sherlock won! Opponent draws 1 card.\n";
            try
            {
                opponent->getDeck()->draw();
                cout << "added to " << opponent->getName() << " hand\n\n";
            }
            catch(const runtime_error& e)
            {
                cout << e.what() << endl;
                for(int i = 0 ; i <  opponent->getfighterCount() ; i++)
                {
                    opponent->getsidekick(i)->takeDamage(2,0);
                }
                cout << "All character on team took 2 damage";
            }
        }
        else
        {
            cout << "You lost or tied! You draw 2 cards.\n";
            try
            {
                self->getDeck()->draw();
                cout << "added to " << self->getName() << " hand\n\n";
            }
            catch(const runtime_error& e)
            {
                cout << e.what() << endl;
                for(int i = 0 ; i <  self->getfighterCount() ; i++)
                {
                    self->getsidekick(i)->takeDamage(2,0);
                }
                cout << "All character on team took 2 damage";
            }
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
    
    else if (card.getName() == "Eliminate the Impossible")
    {
        
    }
    
    else if (card.getName() == "Deception")
    {
        FeintSH = true;
    }
    
    else if (card.getName() == "Fixed Point in a Changing Age")
    {
        
    }
    
    else if (card.getName() == "Master of Disguise")
    {
        
    }
    
    else if (card.getName() == "The Game is Afoot")
    {
        
    }
    
    else if (card.getName() == "Service Revolver")
    {
        
    }
    
    else if (card.getName() == "Study Methods")
    {
        
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