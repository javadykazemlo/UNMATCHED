#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include "core/Controller.hpp"

using namespace std;


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
                    if(bord.getCharacter(self->getsidekick(i)->checkalive()))
                    {
                        if(bord.getCharacter(adjen) == bord.getCharacter(self->getsidekick(i)->getSpace()))
                        {
                            amount++;
                        }
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

        int random = 0;
        if(opponent->getDeck()->gethandSize() != 0)
        {
            random = rand() % opponent->getDeck()->gethandSize();
        }
        
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
                self->getDeck()->showHand(self->getName());
                int handSize = self->getDeck()->gethandSize();
    
                int choice;
                while (true)
                {
                    cout << "choice a card: ";
                    choice = getInt();
                    if(choice > 0 && choice < handSize)
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
                self->getsidekick(i)->takeDamage(2);
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

        card.setAttack(card.getAttack() + enemyboost);

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
                    target->takeDamage(2);

                    if(!target->checkalive())
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
                    opponent->getsidekick(i)->takeDamage(2);
                }
                cout << "All character on team took 2 damage";
            }
        }
        else
        {
            cout << opponent->getName() << " won! " << self->getName() << " draws 2 card.\n";
            try
            {
                self->getDeck()->draw(2);
                cout << "added to " << self->getName() << " hand\n\n";
            }
            catch(const runtime_error& e)
            {
                cout << e.what() << endl;
                for(int i = 0 ; i <  self->getfighterCount() ; i++)
                {
                    self->getsidekick(i)->takeDamage(2);
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
            opponent->getDeck()->showHand(opponent->getName());
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

        cout << "\nEffect >> " << card.geteffect() << endl;

        int pos;
        while(true)
        {
            cout << "Choose a space for Dracula: ";
            pos = getInt();
            if(pos > 0 && pos < 31 && bord.isEmpty(pos))
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

        cout << "\nEffect >> " << card.geteffect() << endl;

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

        cout << "\nEffect >> " << card.geteffect() << endl;

        int amount = 0;
        vector<int> draculaAdjence = bord.getCharacterAdjacent(attacker);
        for(int adjen : draculaAdjence)
        {
            if(!bord.isEmpty(adjen))
            {
                if(bord.getCharacter(adjen)->getowner() != attacker->getowner())
                {
                    bord.getCharacter(adjen)->takeDamage(1);
                    cout << bord.getCharacter(adjen)->getName() << "took 1 damage\n";
                    amount++;
                }
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

        cout << "\nEffect >> " << card.geteffect() << endl;

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
                    if(self->getsidekick(i)->checkalive())
                    {
                        if(bord.getCharacter(adjenc) == self->getsidekick(i))
                        {
                            number++;
                        }
                    }
                }
            }
        }

        selected->takeDamage(number);
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
        
        cout << "\nEffect >> " << card.geteffect() << endl; 

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
                break; 
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
                self->getsidekick(i)->takeDamage(2);
            }
            cout << "All character on team took 2 damage";
        }
        
        return;

    }
    
    else if (card.getName() == "Confirm Suspicion")
    {
        cout << "\nEffect >> " << card.geteffect() << endl; 
        
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
        for(int i = 0 ; i < opponent->getDeck()->gethandSize() ; i++)
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

            opponent->getDeck()->showHand(opponent->getName());
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

        opponent->getHero()->takeDamage(burnBoost);

        cout << opponent->getHero()->getName() << " took " << burnBoost << " damamge" << endl;

        return;

    }

    else if (card.getName() == "Eliminate the Impossible")
    {
        cout << "\nEffect >> " << card.geteffect() << endl; 

        opponent->getDeck()->showHand(opponent->getName());

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
        cout << "\nEffect >> " << card.geteffect() << endl; 

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

        Rival->takeDamage(1);
        cout << Rival->getName() << " took a damage." << endl;
        return;
        
    }

}