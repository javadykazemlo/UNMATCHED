#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include "core/Controller.hpp"
#include "GameTUI.hpp"
#include "entities/invisible_man.hpp"
#include "Save/SaveManager.hpp"

using namespace std;

void Controller::startMenu(Player player[2])
{
    cout << "1. New Game\n2. Load Game\nChoose an option: ";
    int choice = 0;
    choice = getChoice({1,2});

    if (choice == 2)
    {
        cout << "Choose a save slot to load (1-3): ";
        int slot = 0;
        slot = getChoice({1,2,3});
        while (slot < 1 || slot > 3)
        {
            cout << "Invalid slot. Choose 1-3: ";
            slot = getChoice({1,2,3});
        }
        string slotFile = "save" + to_string(slot) + ".json";

        if (LoadGame(player, slotFile))
        {
            cout << "\nResuming saved game...\n";
        }
        else
        {
            cout << "\nNo valid save found in that slot, starting a new game instead.\n";
            GameTUI::showWelcome();
            choosePlayers(player);
        }
    }
    else
    {
        GameTUI::showWelcome();
        choosePlayers(player);
    }
}

void Controller::choosePlayers(Player player[2])
{
    string n;
    int a;

    cout << "\nFirst player, enter your age: ";
    a = getInt();
    player[0].setAge(a);

    cout << "\nFirst player, enter your name: ";
    getline(cin , n);
    player[0].setName(n);

    cout << "\nPlay single-player against the AI? (y/n): ";
    bool singlePlayer = getYesNo();

    if(singlePlayer)
    {
        player[1].setAge(player[0].getAge());
        player[1].setName("AI");
        player[1].setAI(true);

        cout << "\n🤖 Second player is now controlled by the AI.\n";
    }
    else
    {
        cout << "\nSecond player, enter your age: ";
        a = getInt();
        player[1].setAge(a);

        cout << "\nSecond player, enter your name: ";
        getline(cin >> ws , n);
        player[1].setName(n);
    }


    if(singlePlayer || player[0].getAge() <= player[1].getAge())
    {
        current = &player[0];
        enemy = &player[1];
    }
    else
    {
        current = &player[1];
        enemy = &player[0];
    }
    chooseCharacters();
}



void Controller::chooseCharacters()
{
    cout << "\nCharacters:" << endl;
    cout << " 1. Dracula" << endl;
    cout << " 2. Sherlock" << endl;
    cout << " 3. invisible man" << endl;
    
    activeDecider = current;

    int cha_cur;
    cout << current->getName() << ", choose your Character: ";
    cha_cur = getChoice({1,2,3});
    current->chooseCharacter(cha_cur , 1);

    cout << "\nRemaining Characters:" << endl;

    vector<int> available;
    if (cha_cur != 1) {
        cout << " 1. Dracula" << endl;
        available.push_back(1);
    }
    if (cha_cur != 2) {
        cout << " 2. Sherlock" << endl;
        available.push_back(2);
    }
    if (cha_cur != 3) {
        cout << " 3. Invisible Man" << endl;
        available.push_back(3);
    }
    
    activeDecider = enemy;

    int cha_enm;
    cout << enemy->getName() << ", choose your Character: ";
    cha_enm = getChoice(available);
    enemy->chooseCharacter(cha_enm , 2);


    activeDecider = current;

    int pos;
    cout << endl << current->getName() << ", choose your Character position (1 = left map ) (2 = righ map): ";
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
    activeDecider = &player;

    Character* hero = player.getHero();
    vector<int> zone = bord.getCharacterZone(hero);
    vector<int> space  = bord.getEmptyZone(zone);
    vector<int> choose; 
    int s;
    
    if(hero->getName() == "sherlock")
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
    else if(hero->getName() == "Dracula")
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
    else if(hero->getName() == "invisible man")
    {
        invisible_man* im = dynamic_cast<invisible_man*>(hero);

        cout << "\n══════════════════════════════════════════════════════════════════" << endl;
        for(int i = 0 ; i < 3 ; i++)
        {
            cout << "Available space: ";
            for(int j = 0 ; j < space.size()  ; j++)
            {
                if(!bord.getSpaceStatus(space[j]) && !im->isMistPosition(space[j]))
                {
                    cout << "   " << space[j];  
                    choose.push_back(space[j]);
                }
            }
            cout << "\nwhere do you want to place fog token #" << i + 1 << ": ";
            s = getChoice(choose);
            im->setMistToken(i , s);
            choose.clear();
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
            activeDecider = current;

            // نمایش TUI (نقشه و اطلاعات کاراتر ها و اطلاعات کارت ها)
            GameTUI::render(*current, *enemy, current, bord);

            cout << "\n═══════════════════════════════════════════════════════════════════════════════" << endl;
            cout << "                          " << current->getName() << "'s turn\n";
           
            if(current->getHero()->getName() == "Dracula")
            current->getHero()->ability(bord , current);

    
            cout << "\nActions:  \n 1.Maneuver\n 2.Scheme\n 3.Attack\n 4.End Turn\n 5.Save Game";
            cout << "\nChoose a action: ";

            aiDecisionKind = AIDecision::ActionChoice;
            Todo = getChoice({1,2,3,4,5});
            aiDecisionKind = AIDecision::Generic;

            
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
                        damageAllFighters(current, 2);
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
                    aiDecisionKind = AIDecision::FighterSelect;
                    aiCharacterOptions = choices;

                    choose = getChoice({valid});

                    aiDecisionKind = AIDecision::Generic;
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
                case 5:
                {
                    cout << "\nChoose a save slot (1-3): ";
                    int slot = getChoice({1, 2, 3});
                    string slotFile = "save" + to_string(slot) + ".json";
                    SaveGame(slotFile);
                    continue;
                }
                default:
                {
                    break;
                }

                
            }

            for(Character* c : current->getCharacters())
            {
                if(c && !c->checkalive() && c->getSpace() != -1)
                {
                    bord.deletCharacter(c->getSpace());
                    c->setSpace(-1);
                }
            }

            for(Character* c : enemy->getCharacters())
            {
                if(c && !c->checkalive() && c->getSpace() != -1)
                {
                    bord.deletCharacter(c->getSpace());
                    c->setSpace(-1);
                }
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

    validSpaces.push_back(place);
    currently.push_back(place);
    visited[place] = true;

    invisible_man* imMover = dynamic_cast<invisible_man*>(selected);

    while(mov--)
    {
        next.clear();
        for(int currentPos : currently)
        {
            vector<int> neighbors = bord.getposAdjacent(currentPos);
            vector<int> tunnel = bord.getSecretPassages(currentPos);
            neighbors.insert(neighbors.end() , tunnel.begin() , tunnel.end());

            if(imMover != nullptr && imMover->isMistPosition(currentPos))
            {
                for(int mistPos : imMover->getMistTokens())
                {
                    if(mistPos != -1 && mistPos != currentPos)
                        neighbors.push_back(mistPos);
                }
            }
            
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

    aiDecisionKind = AIDecision::MoveDestination;
    aiMovingCharacter = selected;
    int destination = getChoice(validSpaces);
    aiDecisionKind = AIDecision::Generic;
    aiMovingCharacter = nullptr;

    bord.deletCharacter(place);
    bord.addCharacter(destination, selected);
}



int Controller::boost()
{
    cout << "Do you want to use Boost? (y/n): ";

    aiDecisionKind = AIDecision::BoostChoice;
    bool useBoost = getYesNo();
    aiDecisionKind = AIDecision::Generic;

    if (!useBoost)
    {
        cout << "🟥No boost used.\n\n";
        return 0;
    }
    current->getDeck()->showHand(current->getName());
    int choos = current->getDeck()->gethandSize();

    vector<int> valid;
    for(int i = 1 ; i <= choos ; i++)
        valid.push_back(i);

    cout << "Selected card: ";
    aiDecisionKind = AIDecision::CardSelect;
    aiCardPurpose = AICardPurpose::Boost;
    int select = getChoice(valid);
    aiDecisionKind = AIDecision::Generic;

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
        aiDecisionKind = AIDecision::FighterSelect;
        aiCharacterOptions = choices;
        choose = getChoice({valid});
        aiDecisionKind = AIDecision::Generic;
        selected = choices[choose - 1];

        cout << "\nChoose a card: ";
        aiDecisionKind = AIDecision::CardSelect;
        aiCardPurpose = AICardPurpose::Scheme;
        aiCardFighter = selected;
        index = getChoice(choos);
        aiDecisionKind = AIDecision::Generic;
        aiCardFighter = nullptr;
        
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
    cancelEffectIM = false;
    GuessElementary = false;

    if(current->getDeck()->getAttackCardIndices().empty())
    {
        cout << "You have no attack cards. Cannot attack this turn.\n";
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
    aiDecisionKind = AIDecision::FighterSelect;
    aiCharacterOptions = choices;
    choose = getChoice({valid});
    aiDecisionKind = AIDecision::Generic;
    Character* attacker = choices[choose - 1];

    valid.clear();
    choices.clear();
    number = 1;
    vector<Character*> ch = bord.getAttackCharacters(attacker->getAttacktype() , attacker->getSpace());

    cout << "\nEnemy Characters:\n";
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
        aiDecisionKind = AIDecision::AttackTarget;
        aiCharacterOptions = choices;
        choose = getChoice({valid});
        aiDecisionKind = AIDecision::Generic;
        Character* defender = choices[choose - 1];

        Card attackCard = chooseCombatCard(current , attacker , true);
        Card defenseCard = chooseCombatCard(enemy , defender , false);
    
        if(attackCard.getAttack() == 0) 
        {
            cout << "You have no attack cards. Cannot attack this turn.\n";
            return;
        }
        resolveCombat(attackCard, defenseCard , attacker , defender);
    }
    else
    {
        cout << "\nThere are no enemies you can attack.\n";
    }
} 

Card Controller::chooseCombatCard(Player* player , Character* fighter, bool attack)
{
    activeDecider = player;

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
        cout << idx<< "  ";

        cout << "\n> ";

        int choice;
        aiDecisionKind = AIDecision::CardSelect;
        aiCardPurpose = attack ? AICardPurpose::Attack : AICardPurpose::Defense;
        aiCardFighter = fighter;
        choice = getChoice(myhandcard);
        aiDecisionKind = AIDecision::Generic;
        aiCardFighter = nullptr;
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

        for(int idx : myhandcard) 
        {
            if(ownerOK)
            {
                break;
            }
            cout << "This fighter don't have any card to use.\n";
            Card empty;
            return empty;
        }

        
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
        activeDecider = enemy;
        cout << enemy->getName() << ", guess the attack value of your opponent's card.\n";
        int number = getInt();
        if(number == attackCard.getAttack())
        {
            GuessElementary = true;
        }
        activeDecider = current;
    }
    
    int attackValue = attackCard.getAttack();
    int defenseValue = defenseCard.getAttack();

    invisible_man* imDefender = dynamic_cast<invisible_man*>(defender);
    if(imDefender != nullptr && imDefender->isMistPosition(imDefender->getSpace()) && !defenseCard.getName().empty())
    {
        defenseValue += 1;
        cout << "\n🌫️ Invisible Man is on a fog token: defense +1 (cannot be cancelled).\n";
    }

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
        cout << "\n💥 " << current->getName() << " deals " << damage << " damage to " << defender->getName() << "!\n";
        defender->takeDamage(damage);

        if(!defender->checkalive() && defender->getSpace() != -1)
        {
            bord.deletCharacter(defender->getSpace());
            defender->setSpace(-1);
        }

        cout << "  " << defender->getName() << " HP: " << defender->getHp() 
        << "/" << defender->getMaxhp() << "\n";
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
    cancelEffectIM = false;
}


int Controller::getInt()
{
    Player* decider = activeDecider ? activeDecider : current;
    if(decider && decider->isAI())
        return aiInt(decider);

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
    Player* decider = activeDecider ? activeDecider : current;
    if(decider && decider->isAI())
        return aiChoose(valid, decider);

    while(true)
    {
        int x = getInt();
        for(int i : valid)
            if(i == x) return x;
        std::cout << "Invalid choice. Try again: ";
    }
}

bool Controller::getYesNo()
{
    Player* decider = activeDecider ? activeDecider : current;
    if(decider && decider->isAI())
        return aiYesNo(decider);

    char choice;
    cin >> choice;
    return (choice == 'y' || choice == 'Y');
}

int Controller::boardDistance(int from, int to)
{
    if(from == to) return 0;
    if(from < 0 || to < 0) return 99;

    vector<bool> visited(32, false);
    vector<int> currently = { from };
    visited[from] = true;
    int dist = 0;

    while(!currently.empty() && dist < 12)
    {
        dist++;
        vector<int> next;
        for(int pos : currently)
        {
            vector<int> neighbors = bord.getposAdjacent(pos);
            vector<int> tunnel = bord.getSecretPassages(pos);
            neighbors.insert(neighbors.end() , tunnel.begin() , tunnel.end());

            for(int n : neighbors)
            {
                if(n == to) return dist;
                if(!visited[n])
                {
                    visited[n] = true;
                    next.push_back(n);
                }
            }
        }
        currently = next;
    }
    return 99;
}

int Controller::aiScoreAction(int action, Player* decider)
{
    bool canAttackAny = false;
    for(Character* ch : decider->getCharacters())
        if(ch && ch->checkalive() && bord.canAttack(ch->getAttacktype() , ch->getSpace()))
            canAttackAny = true;

    bool hasAttackCard = !decider->getDeck()->getAttackCardIndices().empty();
    bool hasSchemeCard = !decider->getDeck()->getSchemeCardIndices().empty();

    switch(action)
    {
        case 3:
            return (canAttackAny && hasAttackCard) ? 100 : -50;
        case 2: 
            return hasSchemeCard ? 55 : -50;
        case 1:
            return 40;
        case 4:
            return 5;
    }
    return 0;
}

int Controller::aiScoreFighter(int idx, Player* decider)
{
    if(idx < 1 || idx > (int)aiCharacterOptions.size())
        return -1000;

    Character* ch = aiCharacterOptions[idx - 1];
    if(!ch) return -1000;

    int score = ch->getHp();
    if(ch->isHero())
        score += 5;
    return score;
}

int Controller::aiScoreMove(int destination, Player* decider)
{
    Player* opp = (decider == current) ? enemy : current;
    Character* mover = aiMovingCharacter;
    Character* enemyHero = opp ? opp->getHero() : nullptr;

    if(!mover || !enemyHero || enemyHero->getSpace() == -1)
        return 0;

    int dist = boardDistance(destination, enemyHero->getSpace());
    bool lowHp = mover->getHp() <= mover->getMaxhp() / 3;

    if(lowHp)
        return min(dist, 6) * 10; 

    int score = 100 - min(dist, 10) * 8; 
    if(bord.canAttack(mover->getAttacktype() , destination))
        score += 30; 

    return score;
}

int Controller::aiScoreAttackTarget(int idx, Player* decider)
{
    if(idx < 1 || idx > (int)aiCharacterOptions.size())
        return -1000;

    Character* target = aiCharacterOptions[idx - 1];
    if(!target) return -1000;

    int score = 0;
    if(target->isHero())
        score += 60; 
    score += (target->getMaxhp() - target->getHp()) * 3; 
    score += max(0 , 20 - target->getHp());

    return score;
}

int Controller::aiScoreCardChoice(int idx, Player* decider)
{
    const vector<Card>& hand = decider->getDeck()->gethand();
    if(idx < 1 || idx > (int)hand.size())
        return -1000;

    const Card& c = hand[idx - 1];

    if(aiCardFighter)
    {
        bool ownerOK = aiCardFighter->isHero() ? (c.isHero() || c.isAnyowner())
                                                : (c.issideKick() || c.isAnyowner());
        if(!ownerOK)
            return -1000;

        if(aiCardPurpose == AICardPurpose::Attack && !(c.isAttack() || c.isVersatile()))
            return -1000;
        if(aiCardPurpose == AICardPurpose::Defense && !(c.isDefense() || c.isVersatile()))
            return -1000;
    }

    switch(aiCardPurpose)
    {
        case AICardPurpose::Attack:
            return c.getAttack() * 10; 
        case AICardPurpose::Defense:
            return c.getAttack() * 10;
        case AICardPurpose::Boost:
            return c.getBoost() * 6 - c.getAttack() * 4;
        case AICardPurpose::Scheme:
            return c.getBoost() * 8 + c.getAttack() * 2;
    }
    return 0;
}

void Controller::aiThink(Player* decider)
{
    cout << "🤖 " << decider->getName() << " is thinking";
    cout.flush();

    int ms = 2500 + (rand() % 1500);
    int dots = 4;
    for(int i = 0 ; i < dots ; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms / dots));
        cout << ".";
        cout.flush();
    }
    cout << "\n";
}

int Controller::aiChoose(const std::vector<int>& valid, Player* decider)
{
    if(valid.empty())
        return 0;

    aiThink(decider);

    int bestScore = std::numeric_limits<int>::min();
    vector<int> best;

    for(int v : valid)
    {
        int score;
        switch(aiDecisionKind)
        {
            case AIDecision::ActionChoice:       score = aiScoreAction(v, decider);       break;
            case AIDecision::FighterSelect:      score = aiScoreFighter(v, decider);      break;
            case AIDecision::MoveDestination:    score = aiScoreMove(v, decider);         break;
            case AIDecision::AttackTarget:       score = aiScoreAttackTarget(v, decider); break;
            case AIDecision::CardSelect:         score = aiScoreCardChoice(v, decider);   break;
            default:                             score = rand() % 100;                    break;
        }

        if(score > bestScore)
        {
            bestScore = score;
            best.clear();
            best.push_back(v);
        }
        else if(score == bestScore)
        {
            best.push_back(v);
        }
    }

    int pick = best[rand() % best.size()];
    cout << "🤖 " << decider->getName() << " (AI) chooses: " << pick << "\n";
    return pick;
}

bool Controller::aiYesNo(Player* decider)
{
    aiThink(decider);

    bool yes;

    if(aiDecisionKind == AIDecision::BoostChoice)
    {
        const vector<Card>& hand = decider->getDeck()->gethand();
        bool handFull = hand.size() >= 6;
        bool hasSpareCard = false;
        for(const Card& c : hand)
            if(c.getAttack() <= 1) { hasSpareCard = true; break; }

        yes = handFull || hasSpareCard;
    }
    else
    {
        yes = (rand() % 2) == 0;
    }

    cout << "🤖 " << decider->getName() << " (AI) answers: " << (yes ? "yes" : "no") << "\n";
    return yes;
}

int Controller::aiInt(Player* decider)
{
    aiThink(decider);

    int x = rand() % 7 + 1;
    cout << "🤖 " << decider->getName() << " (AI) enters: " << x << "\n";
    return x;
}

void Controller::damageAllFighters(Player* p, int damage)
{
    if(!p) return;

    for(Character* c : p->getCharacters())
    {
        if(c && c->checkalive())
        {
            c->takeDamage(damage);
            if(!c->checkalive() && c->getSpace() != -1)
            {
                bord.deletCharacter(c->getSpace());
                c->setSpace(-1);
            }
        }
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


void Controller::SaveGame(const string& filename)
{
    if (SaveManager::saveGame(current, enemy, gamerand,
                               cancelEffectDR, cancelEffectSH, cancelEffectIM, GuessElementary,
                               filename))
        cout << "\n✅ Game saved to \"" << filename << "\".\n";
    else
        cout << "\n❌ Failed to save the game.\n";
}

bool Controller::LoadGame(Player player[2], const string& filename)
{
    Player* loadedCurrent = nullptr;
    Player* loadedEnemy = nullptr;
    int loadedGamerand = 0;
    bool loadedCancelDR = false;
    bool loadedCancelSH = false;
    bool loadedCancelIM = false;
    bool loadedGuessElementary = false;

    if (!SaveManager::loadGame(bord, player, loadedCurrent, loadedEnemy,
                                loadedGamerand, loadedCancelDR, loadedCancelSH,
                                loadedCancelIM, loadedGuessElementary, filename))
    {
        cout << "\n❌ Failed to load the game from \"" << filename << "\".\n";
        return false;
    }

    current = loadedCurrent;
    enemy = loadedEnemy;

    cancelEffectDR = loadedCancelDR;
    cancelEffectSH = loadedCancelSH;
    cancelEffectIM = loadedCancelIM;
    gamerand = loadedGamerand;
    GuessElementary = loadedGuessElementary;

    cout << "\n✅ Game loaded from \"" << filename << "\".\n";
    return true;
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