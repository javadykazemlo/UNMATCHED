#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include "core/Controller.hpp"

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
    cout << current->getName() << ", choose your Character position(4 or 15) 4 = (left map ) (15 = righ map): ";
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
    int choos = current->getDeck()->gethandSize();
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
    
    vector<int> choos = current->getDeck()->getSchemeCardIndices();
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

    applyEffectScheme(Schemecard , current , enemy , selected);

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
    if(defenseCard.getName() == "Elementary")
    {
        cout << enemy->getName() << ", guess the attack value of your opponent's card.\n";
        int number = getInt();
        if(number == attackCard.getAttack())
        {
            GuessElementary = true;
        }
    }
    
    int attackValue = attackCard.getAttack();;
    int defenseValue = attackCard.getAttack();;
    
    
    cout << "\n═══════════════════════════════════════════════════════\n";
    cout << "  ⚔️ RESOLVING COMBAT ⚔️\n";
    cout << "═══════════════════════════════════════════════════════\n";
    
    
    if(defenseCard.isBeforeCombat())
    {
        applyEffect(defenseCard , attackCard , enemy , current , attacker , defender , false);
    } 
    if(attackCard.isBeforeCombat())
    {
        applyEffect(attackCard , defenseCard , current , enemy , attacker , defender , false);
    }

    cout << "\n📊 COMBAT RESULT:\n";
    cout << "  ⚔️ Attack  : " << attackValue << "\n";
    cout << "  🛡️ Defense : " << defenseValue << "\n";
    
    
    if(defenseCard.isDuringCombat())
    {
        applyEffect(defenseCard , attackCard , enemy , current , attacker , defender , false);
    }
    if(attackCard.isDuringCombat())
    {
        applyEffect(attackCard , defenseCard , current , enemy , attacker , defender , false);
    }
    
    bool attackerWon = false;
    bool defenderWon = false;

    if (attackValue > defenseValue) 
    {
        int damage = attackValue - defenseValue;
        cout << "\n💥 " << current->getName() << " deals " << damage << " damage!\n";
        enemy->getHero()->takeDamage(defenseValue, attackValue);

        if(!enemy->getHero()->getIsAlive())
        bord.deletCharacter(enemy->getHero()->getSpace());

        cout << "  " << enemy->getName() << " HP: " << enemy->getHero()->getHp() 
        << "/" << enemy->getHero()->getMaxhp() << "\n";
        attackerWon = true;
    }
    else if (attackValue <= defenseValue) 
    {
        cout << "\n🛡️ " << enemy->getName() << " blocks the attack!\n";
        defenderWon = true;
    }


    
    if(defenseCard.isAfterCombat())
    {
        applyEffect(defenseCard , attackCard , enemy , current , attacker , defender , defenderWon);
    }
    if(attackCard.isAfterCombat())
    {
        applyEffect(attackCard , defenseCard , current , enemy , attacker , defender , attackerWon);
    }

    cancelEffectDR = false;
    cancelEffectSH = false;
}

int Controller::getInt()
{
    std::string line;
    int x;
    
    while (true)
    {
        std::getline(std::cin, line);
        
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
        
        if (line.empty()) {
            std::cout << "Enter a number: ";
            continue;
        }
        
        try {
            x = std::stoi(line);
            return x;
        }
        catch (const std::exception& e) {
            std::cout << "Invalid input. Enter a number: ";
        }
    }
}

int Controller::getChoice(std::vector<int> valid)
{
    while(true)
    {
        int x = getInt();

        for(int i : valid)
        {
            if(i == x) return x;
        }

        std::cout << "Invalid choice. Try again: ";
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



Player* Controller::getCurrentPlayer() const 
{
     return current;
}


Player* Controller::getEnemyPlayer() const 
{ 
    return enemy; 
}


Bord& Controller::getBoard() 
{ 
    return bord; 
}


const Bord& Controller::getBoard() const 
{ 
    return bord; 
}


bool Controller::isGameOver() const 
{ 
    return end_game(); 
}


Character* Controller::getCharacterAt(int position) const 
{ 
    return bord.getCharacter(position); 
}




void Controller::applyEffect(Card& card , Card& enemycard ,Player* self, Player* opponent , Character* attacker , Character* defender , bool woncombat)
{
//<<<<<<<<<<<<<<<<< Dracula

    if (card.getName() == "Feeding Frenzy")
    {
        if (cancelEffectDR)
        {
            cout << "Feeding Frenzy effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

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

        return;

    }
    
    else if (card.getName() == "Ambush")
    {
        if (cancelEffectDR)
        {
            cout << "Ambush effect was canceled.\n";
            return;
        } 

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
    
    else if (card.getName() == "BeastForm")
    {
        if (cancelEffectDR)
        {
            cout << "BeastForm effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        while(true)
        {
            cout << "Do you want to remove a card?(y/n): ";
            char option;
            cin >> option;
            if(option == 'y' || option == 'Y')
            {
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
        if (cancelEffectDR)
        {
            cout << "Dash effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        move(3 , attacker);

        return;
        
    }
    
    else if (card.getName() == "Exploit")
    {
        if (cancelEffectDR)
        {
            cout << "Exploit effect was canceled.\n";
            return;
        }

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
        if (cancelEffectDR)
        {
            cout << "Look Into My Eyes effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        int enemyboost = enemycard.getBoost();

        card.setAttack(enemyboost);

        cout << "This card gained " << enemyboost << " defense.\n";

        return;
        
    }
    
    else if (card.getName() == "Thirst for Sustenance")
    {
        if (cancelEffectDR)
        {
            cout << "Thirst for Sustenance effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        if(woncombat)
        {
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
        }
        
        return;

    }
    
    else if (card.getName() == "Feint" && self->getHero()->getName() == "Dracula")
    {
        if (cancelEffectDR)
        {
            cout << "Feint effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl; 

        cancelEffectSH = true;
    
        return;
        
    }

//<<<<<<<<<<<<<<<<< Sherlock
    
    else if (card.getName() == "Counterpunch")
    { 
        cout << card.geteffect() << endl; 

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
        
        return;

    }
    
    else if (card.getName() == "Deduce Strategy")
    {
        cout << card.geteffect() << endl; 

        int enemyboost = enemycard.getBoost();
        enemycard.setAttack(enemyboost);

        cout << "The opponent's card " << enemycard.getTypeString() << " was changed to " << enemyboost << ".\n";

        return;

    }
    
    else if (card.getName() == "Education Never Ends")
    {
        if (cancelEffectSH)
        {
            cout << "Education Never Ends effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl; 
        
        if (woncombat)
        {
            cout << self->getName() << " won! " << opponent->getName() << " draws 1 card.\n";
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
            cout << opponent->getName() << " won! " << self->getName() << " draws 2 card.\n";
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

        return;
        
    }
    
    else if (card.getName() == "Elementary")
    {
        cout << card.geteffect() << endl; 
        
        if (GuessElementary)
        {
            cout << self->getName() << " guessed -> succeessful";

            enemycard.setAttack(0);
            cancelEffectDR = true;

            cout << "All effects on the " << opponent->getName() << " card were removed, and its attack value was ignored.\n";
        }
        else
        {
            cout << self->getName() << " guessed -> failed";
        }

        GuessElementary = false;

        return;

    }
    
    else if (card.getName() == "Feint" && self->getHero()->getName() == "sherlock")
    {
        if (cancelEffectSH)
        {
            cout << "Feint effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl; 

        cancelEffectDR = true;
        
        return;

    }
    
    else if (card.getName() == "Fixed Point in a Changing Age")
    {
        cout << card.geteffect() << endl; 

        Character* holmes = self->getHero();
        Character* watson = self->getsidekick(1);

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

        return;

    }
    
    else if (card.getName() == "The Game is Afoot")
    {
        cout << card.geteffect() << endl; 

        Character* holmes = self->getHero();
        move(3 , holmes);

        return;

    }
    
    else if (card.getName() == "Service Revolver")
    {

        cout << "no effect (-_-)";

        return;

    }
    
    else if (card.getName() == "Study Methods")
    {
        if (cancelEffectSH)
        {
            cout << "Study Methods effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl; 

        if (woncombat)
        {
            cout << "Sherlock won the combat.\n";
            cout << "Opponent's hand:\n";
        }
        else
        {
            cout << "Card effect not activated.\n";
        }
        
        return;

    }
}    


void Controller::applyEffectScheme(Card& card ,Player* self, Player* opponent , Character* attacker )
{
//<<<<<<<<<<<<<<<<< Dracula 

    if (card.getName() == "MistForm")
    {
        if (cancelEffectDR)
        {
            cout << "MistForm effect was canceled.\n";
            return;
        }

        cout << "Effect >> " << endl << card.geteffect() << endl;

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

    else if (card.getName() == "Baptism of Blood")
    {
        if (cancelEffectDR)
        {
            cout << "Baptism of Blood effect was canceled.\n";
            return;
        }

        cout << "Effect >> " << endl << card.geteffect() << endl;

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

    else if (card.getName() == "Prey Upon")
    {
        if (cancelEffectDR)
        {
            cout << "Prey Upon effect was canceled.\n";
            return;
        }

        cout << "Effect >> " << endl << card.geteffect() << endl;

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
        if (cancelEffectDR)
        {
            cout << "Ravening Seduction effect was canceled.\n";
            return;
        }

        cout << "Effect >> " << endl << card.geteffect() << endl;

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

//<<<<<<<<<<<<<<<<< sherlok

    else if (card.getName() == "Administer Aid")
    {
        
        cout << "Effect >> " << endl << card.geteffect() << endl; 

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

        for (int i = 0; i < emptyAdjacent.size() ; i++)
        {
            if(bord.isEmpty(emptyAdjacent[i]))
            {
                bord.addCharacter(emptyAdjacent[i],watson);      
            }
            
        }
        
          
        holmes->heal(1);
        cout << "Holmes healed 1 HP \n";
        
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
    
    else if (card.getName() == "Confirm Suspicion")
    {
        cout << "Effect >> " << endl << card.geteffect() << endl; 
        
        int choose = 0;
        vector<Character*> choices;
        vector<int> valid;
        int k = 1;

        for (Character* ch : opponent->getCharacters())
        {
            if(ch->checkalive())
            {
                cout << k << "." << ch->getName() << endl;
                choices.push_back(ch);
                valid.push_back(k);
                k++;
            }
        }
        cout << "Choose an opponent: ";
        choose = getChoice({valid});
        Character* Rival = choices[choose - 1];

        cout << "Choose a number: ";
        int number = getInt();

        bool isExist = false;
        for(int i = 0 ; i < (opponent->getDeck()->gethandSize() + 1) ; i++)
        {
            if(number == opponent->getDeck()->getHandcard(i).getAttack())
            {
                isExist = true;
                break;
            }
        }

        if(!isExist)
        {
            cout << "The opponent has no card with an attack or defense value of " << number << ".\n";

            return;
        }
        
        Card burn;
        while (true)
        {
            cout << opponent->getName() << ", choose a card with " << number << "attack or defense: ";
            int select = getInt();
            if(select > 0 && select < (opponent->getDeck()->gethandSize() + 1))
            {
                if(number == opponent->getDeck()->getHandcard(select - 1).getAttack())
                {
                    burn = opponent->getDeck()->playCard(select - 1 , burn);
                    break;
                }
            }
            cout << "Invalid input.\n";
        }
        
        
        int burnBoost = burn.getBoost();

        opponent->getHero()->takeDamage(0 , burnBoost);

        cout << opponent->getHero()->getName() << " took " << burnBoost << " damamge" << endl;

        return;

    }

    else if (card.getName() == "Eliminate the Impossible")
    {
        cout << "Effect >> " << endl << card.geteffect() << endl; 


        int index;
        while (true)
        {
            cout << "Choose a card to burn: ";
            index = getInt();
            if(index > 0 && index < (opponent->getDeck()->gethandSize() + 1))
            {
                break;
            }
            cout << "invalid input.";
        }
        

        Card burned;
        burned = opponent->getDeck()->playCard(index - 1 , burned);
        cout << burned.getName() <<  " was butn" << endl;

        return;

    }

    else if (card.getName() == "Master of Disguise")
    {
        cout << "Effect >> " << endl << card.geteffect() << endl; 

        Character* holmes = self->getHero();

        int choose = 0;
        vector<Character*> choices;
        vector<int> valid;
        int number = 1;

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
        cout << "Choose a character to swap positions with: ";
        choose = getChoice({valid});
        Character* Rival = choices[choose - 1];

        int holmespos = holmes->getSpace();
        int enemypos = Rival->getSpace();

        bord.deletCharacter(holmespos);
        bord.deletCharacter(enemypos);

        bord.addCharacter(enemypos , holmes);
        bord.addCharacter(holmespos , Rival);

        Rival->takeDamage(0 , 1);
        cout << Rival->getName() << " took a damage." << endl;
        return;
        
    }

}


bool Controller::get_DraculaWon() {
    return false;
}
bool Controller::get_SherlockWon() {
    return false;
}
bool Controller::getCancelEffect() {
    return cancelEffectDR;
}
Controller::~Controller()
{
}