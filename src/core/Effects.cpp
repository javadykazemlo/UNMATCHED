#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include "core/Controller.hpp"
#include "core/GuiEffectLogger.hpp"
#include "entities/invisible_man.hpp"

using namespace std;


void Controller::applyEffect(Card& card , Card& enemycard ,Player* self, Player* opponent , Character* attacker , Character* defender , bool woncombat)
{
    GuiEffectLogger effectLog([this](const std::string& message) { guiLog(message); });
    activeDecider = self;

//<<<<<<<<<<<<<<<<< Dracula

    if (card.getName() == "Feeding Frenzy")
    {
        if (cancelEffectDR)
        {
            effectLog << "Feeding Frenzy effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        vector<int> defendZones = bord.getCharacterZone(defender);

        int amount = 0;

        for(int i = 1 ; i < self->getfighterCount() ; i++)
        {
            Character* sister = self->getFighter(i);
            if(sister == nullptr || !sister->checkalive())
                continue;

            vector<int> sisterZones = bord.getposZone(sister->getSpace());
            for(int z : sisterZones)
            {
                if(find(defendZones.begin() , defendZones.end() , z) != defendZones.end())
                {
                    amount++;
                    break;
                }
            }
        }
        effectLog << "Card " << card.getName() << "'s attack increased by " << amount << ".\n";
        card.setAttack(card.getAttack() + amount);

        return;

    }
    
    else if (card.getName() == "Ambush")
    {
        if (cancelEffectDR)
        {
            effectLog << "Ambush effect was canceled.\n";
            return;
        } 

        effectLog << card.geteffect() << endl;

        int random = 0;
        if(opponent->getDeck()->gethandSize() != 0)
        {
            random = rand() % opponent->getDeck()->gethandSize();
        }
        
        Card boostCard;
        boostCard = opponent->getDeck()->playCard(random , boostCard);

        effectLog << "Card " << boostCard.getName() << " was removed from " << opponent->getName() << "'s hand.\n";

        int boost = boostCard.getBoost();
        
        card.setAttack(boost + card.getAttack());

        effectLog << boostCard.getBoost() << " boost was added to " << card.getName() << "'s attack.\n";

        return;

    }
    
    else if (card.getName() == "BeastForm")
    {
        if (cancelEffectDR)
        {
            effectLog << "BeastForm effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        while(true)
        {
            effectLog << "Do you want to remove a card?(y/n): ";
            if(getYesNo())
            {
                int handSize = self->getDeck()->gethandSize();
    
                int choice;
                while (true)
                {
                    effectLog << "choice a card: ";
                    choice = getInt();
                    if(choice > 0 && choice <= handSize)
                    {
                        break;
                    }
                    effectLog << "Invalid input.";
                }

                Card selected;
                selected = self->getDeck()->playCard(choice - 1 , selected);
    
                card.setAttack(card.getAttack() + 1);
                effectLog << "Card " << card.getName() << " gained +1 attack.\n";
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
            effectLog << "Dash effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        move(3 , attacker);

        return;
        
    }
    
    else if (card.getName() == "Exploit")
    {
        if (cancelEffectDR)
        {
            effectLog << "Exploit effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        try
        {
            self->getDeck()->draw();
            effectLog << "added to " << self->getName() << " hand\n\n";
        }
        catch(const runtime_error& e)
        {
            effectLog << e.what() << endl;
            damageAllFighters(self, 2);
            effectLog << "All character on team took 2 damage";
        }

        return;
        
    }
    
    else if (card.getName() == "Look Into My Eyes")
    {
        if (cancelEffectDR) 
        {
            effectLog << "Look Into My Eyes effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        int enemyboost = enemycard.getBoost();

        card.setAttack(card.getAttack() + enemyboost);

        effectLog << "This card gained " << enemyboost << " defense.\n";

        return;
        
    }
    
    else if (card.getName() == "Thirst for Sustenance")
    {
        if (cancelEffectDR)
        {
            effectLog << "Thirst for Sustenance effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        if(woncombat)
        {
            int place = defender->getSpace();
    
            vector<int> validSpaces = bord.getEmptyAdjacent(defender);

            effectLog << "\nAvailable spaces:   ";

            for(int pos : validSpaces)
            {
                effectLog << pos << "   ";
            }
            effectLog << "\nSelect a destination: ";

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
            effectLog << "Feint effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl; 

        string oppName = opponent->getHero()->getName();
        if (oppName == "sherlock") cancelEffectSH = true;
        else if (oppName == "invisible man") cancelEffectIM = true;
    
        return;
        
    }

//<<<<<<<<<<<<<<<<< Sherlock
    
    else if (card.getName() == "Counterpunch")
    { 
        effectLog << card.geteffect() << endl; 

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

                    effectLog << "2 damage dealt to " << target->getName() << "!\n";
                    damaged = true;
                    break;  
                }
            }
        }
        
        if (!damaged)
        {
            effectLog << "No adjacent enemy fighter!\n";
        }
        
        return;

    }
    
    else if (card.getName() == "Deduce Strategy")
    {
        effectLog << card.geteffect() << endl; 

        int enemyboost = enemycard.getBoost();
        enemycard.setAttack(enemyboost);

        effectLog << "The opponent's card " << enemycard.getTypeString() << " was changed to " << enemyboost << ".\n";

        return;

    }
    
    else if (card.getName() == "Education Never Ends")
    {
        if (cancelEffectSH)
        {
            effectLog << "Education Never Ends effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl; 
        
        if (woncombat)
        {
            effectLog << self->getName() << " won! " << opponent->getName() << " draws 1 card.\n";
            try
            {
                opponent->getDeck()->draw();
                effectLog << "added to " << opponent->getName() << " hand\n\n";
            }
            catch(const runtime_error& e)
            {
                effectLog << e.what() << endl;
                damageAllFighters(opponent, 2);
                effectLog << "All character on team took 2 damage";
            }
        }
        else
        {
            effectLog << opponent->getName() << " won! " << self->getName() << " draws 2 card.\n";
            try
            {
                self->getDeck()->draw(2);
                effectLog << "added to " << self->getName() << " hand\n\n";
            }
            catch(const runtime_error& e)
            {
                effectLog << e.what() << endl;
                damageAllFighters(self, 2);
                effectLog << "All character on team took 2 damage";
            }
        }

        return;
        
    }
    
    else if (card.getName() == "Elementary")
    {
        effectLog << card.geteffect() << endl; 
        
        if (GuessElementary)
        {
            effectLog << self->getName() << " guessed -> succeessful";

            enemycard.setAttack(0);
            cancelEffectDR = true;

            effectLog << "All effects on the " << opponent->getName() << " card were removed, and its attack value was ignored.\n";
        }
        else
        {
            effectLog << self->getName() << " guessed -> failed";
        }

        GuessElementary = false;

        return;

    }
    
    else if (card.getName() == "Feint" && self->getHero()->getName() == "sherlock")
    {
        if (cancelEffectSH)
        {
            effectLog << "Feint effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl; 

        string oppName = opponent->getHero()->getName();
        if (oppName == "Dracula") cancelEffectDR = true;
        else if (oppName == "invisible man") cancelEffectIM = true;
        
        return;

    }
    
    else if (card.getName() == "Fixed Point in a Changing Age")
    {
        effectLog << card.geteffect() << endl; 

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

                effectLog << "Holmes and Watson each heal 1 HP.\n";
                found = true;
                break;
            }
        }

        if (!found)
        {
            effectLog << "Watson is not adjacent to Holmes.\n";
        }

        return;

    }
    
    else if (card.getName() == "The Game is Afoot")
    {
        effectLog << card.geteffect() << endl; 

        Character* holmes = self->getHero();
        move(3 , holmes);

        return;

    }
    
    else if (card.getName() == "Service Revolver")
    {

        effectLog << "no effect (-_-)";

        return;

    }
    
    else if (card.getName() == "Study Methods")
    {
        if (cancelEffectSH)
        {
            effectLog << "Study Methods effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl; 

        if (woncombat)
        {
            effectLog << "Sherlock won the combat.\n";
            effectLog << "Opponent's hand:\n";
        }
        else
        {
            effectLog << "Card effect not activated.\n";
        }
        
        return;

    }

//<<<<<<<<<<<<<<<<< Invisible Man

    else if (card.getName() == "Coded Notes")
    {
        if (cancelEffectIM)
        {
            effectLog << "Coded Notes effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        try
        {
            self->getDeck()->draw(3);
            effectLog << "3 cards added to " << self->getName() << "'s hand.\n";
        }
        catch(const runtime_error& e)
        {
            effectLog << e.what() << endl;
            damageAllFighters(self, 2);
            effectLog << "All character on team took 2 damage";
        }

        int handSize = self->getDeck()->gethandSize();
        effectLog << "Choose the first card to put on top of your deck: ";
        int first = getInt();
        while(first <= 0 || first > handSize)
        {
            effectLog << "Invalid card number. Try again: ";
            first = getInt();
        }
        Card firstCard = self->getDeck()->removeFromHand(first - 1);

        handSize = self->getDeck()->gethandSize();
        effectLog << "Choose the second card to put on top of your deck: ";
        int second = getInt();
        while(second <= 0 || second > handSize)
        {
            effectLog << "Invalid card number. Try again: ";
            second = getInt();
        }
        Card secondCard = self->getDeck()->removeFromHand(second - 1);

        self->getDeck()->putOnTop(secondCard);
        self->getDeck()->putOnTop(firstCard);

        effectLog << firstCard.getName() << " and " << secondCard.getName() << " were placed on top of your deck.\n";

        return;

    }

    else if (card.getName() == "Confound")
    {
        if (cancelEffectIM)
        {
            effectLog << "Confound effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        effectLog << opponent->getName() << ", do you want to discard 1 card? (y/n): ";
        activeDecider = opponent;
        bool willDiscard = getYesNo();

        if (willDiscard)
        {
            int handSize = opponent->getDeck()->gethandSize();

            effectLog << "Choose a card to discard: ";
            int idx = getInt();
            while(idx <= 0 || idx > handSize)
            {
                effectLog << "Invalid card number. Try again: ";
                idx = getInt();
            }

            Card discarded;
            discarded = opponent->getDeck()->playCard(idx - 1 , discarded);
            effectLog << discarded.getName() << " was discarded.\n";
            activeDecider = self;
        }
        else
        {
            activeDecider = self;
            invisible_man* im = dynamic_cast<invisible_man*>(self->getHero());
            if (im != nullptr)
            {
                effectLog << "You may move each fog token to any other space.\n";
                for (int i = 0; i < 3; i++)
                {
                    int tokenPos = im->getMistToken(i);
                    if (tokenPos == -1) continue;

                    vector<int> options;
                    for (int j = 0; j < 32; j++)
                        if (bord.isEmpty(j) && !im->isMistPosition(j))
                            options.push_back(j);
                    options.push_back(tokenPos);

                    effectLog << "\nMove fog token at " << tokenPos << " to (or keep at " << tokenPos << "): ";
                    for (int s : options) effectLog << "   " << s;
                    int dest = getChoice(options);
                    im->setMistToken(i , dest);
                }
            }
        }

        return;

    }

    else if (card.getName() == "Covert Preparation")
    {
        if (cancelEffectIM)
        {
            effectLog << "Covert Preparation effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        try
        {
            self->getDeck()->draw();
            effectLog << "1 card added to " << self->getName() << "'s hand.\n";
        }
        catch(const runtime_error& e)
        {
            effectLog << e.what() << endl;
            damageAllFighters(self, 2);
            effectLog << "All character on team took 2 damage";
        }

        invisible_man* im = dynamic_cast<invisible_man*>(self->getHero());
        if (im == nullptr) return;

        vector<int> tokens;
        for (int t : im->getMistTokens()) if (t != -1) tokens.push_back(t);

        if (tokens.size() >= 1)
        {
            effectLog << "Choose a fog token to move: ";
            for (int t : tokens) effectLog << "   " << t;
            int tokenPos = getChoice(tokens);

            vector<int> emptySpaces;
            for (int i = 0; i < 32; i++)
                if (bord.isEmpty(i) && !im->isMistPosition(i))
                    emptySpaces.push_back(i);

            effectLog << "\nMove it to: ";
            for (int s : emptySpaces) effectLog << "   " << s;
            int dest = getChoice(emptySpaces);

            for (int i = 0; i < 3; i++)
                if (im->getMistToken(i) == tokenPos) { im->setMistToken(i , dest); break; }
        }

        if (tokens.size() >= 2)
        {
            activeDecider = opponent;

            effectLog << "\n" << opponent->getName() << ", choose a different fog token to move: ";
            for (int t : tokens) effectLog << "   " << t;
            int tokenPos2 = getChoice(tokens);

            vector<int> emptySpaces2;
            for (int i = 0; i < 32; i++)
                if (bord.isEmpty(i) && !im->isMistPosition(i))
                    emptySpaces2.push_back(i);

            effectLog << "\nMove it to: ";
            for (int s : emptySpaces2) effectLog << "   " << s;
            int dest2 = getChoice(emptySpaces2);

            for (int i = 0; i < 3; i++)
                if (im->getMistToken(i) == tokenPos2) { im->setMistToken(i , dest2); break; }

            activeDecider = self;
        }

        return;

    }

    else if (card.getName() == "Dreaming of Revenge")
    {
        if (cancelEffectIM)
        {
            effectLog << "Dreaming of Revenge effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(self->getHero());
        if (im == nullptr || !im->isMistPosition(im->getSpace()))
        {
            effectLog << "Invisible Man is not on a fog token.\n";
            return;
        }

        int amount = 0;
        for (Character* ch : opponent->getCharacters())
        {
            if (ch->checkalive() && im->isMistPosition(ch->getSpace()))
            {
                ch->takeDamage(1);
                effectLog << ch->getName() << " took 1 damage.\n";

                if (!ch->checkalive() && ch->getSpace() != -1)
                {
                    bord.deletCharacter(ch->getSpace());
                    ch->setSpace(-1);
                }
                amount++;
            }
        }

        if (amount == 0)
            effectLog << "No opposing fighters are on fog tokens.\n";

        return;

    }

    else if (card.getName() == "Emerge from Mist")
    {
        if (cancelEffectIM)
        {
            effectLog << "Emerge from Mist effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(attacker);
        if (im != nullptr && im->isMistPosition(im->getSpace()))
        {
            card.setAttack(5);
            effectLog << "This card's value becomes 5.\n";
        }

        return;

    }

    else if (card.getName() == "Impossible to See")
    {
        effectLog << card.geteffect() << endl;

        enemycard.setAttack(0);

        string oppName = opponent->getHero()->getName();

        if (oppName == "Dracula") cancelEffectDR = true;
        else if (oppName == "sherlock") cancelEffectSH = true;
        else if (oppName == "invisible man") cancelEffectIM = true;

        effectLog << "The opponent's card value is now 0 and cannot be changed by card effects.\n";

        return;

    }

    else if (card.getName() == "Into Thin Air")
    {
        if (cancelEffectIM)
        {
            effectLog << "Into Thin Air effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(defender);
        if (im == nullptr) return;

        move(1 , im);

        vector<int> tokens;
        for (int t : im->getMistTokens()) if (t != -1) tokens.push_back(t);

        if (!tokens.empty())
        {
            activeDecider = opponent;

            effectLog << "\n" << opponent->getName() << ", choose a fog token to move: ";
            for (int t : tokens) effectLog << "   " << t;
            int tokenPos = getChoice(tokens);

            vector<int> emptySpaces;
            for (int i = 0; i < 32; i++)
            {
                if (bord.isEmpty(i) && !im->isMistPosition(i))
                    emptySpaces.push_back(i);
            }

            effectLog << "\nMove it to: ";
            for (int s : emptySpaces) effectLog << "   " << s;

            int dest = getChoice(emptySpaces);

            for (int i = 0; i < 3; i++)
            {
                if (im->getMistToken(i) == tokenPos) { im->setMistToken(i , dest); break; }
            }

            activeDecider = self;
        }

        return;

    }

    else if (card.getName() == "Lurking")
    {
        if (cancelEffectIM)
        {
            effectLog << "Lurking effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        try
        {
            self->getDeck()->draw();
            effectLog << "1 card added to " << self->getName() << "'s hand.\n";
        }
        catch(const runtime_error& e)
        {
            effectLog << e.what() << endl;
            damageAllFighters(self, 2);
            effectLog << "All character on team took 2 damage";
        }

        invisible_man* im = dynamic_cast<invisible_man*>(defender);
        if (im == nullptr) return;

        effectLog << "\nChoose an effect:\n 1. Move Invisible Man to a fog token space\n 2. Move 1 fog token up to 3 spaces\nChoice: ";
        int choice = getChoice({1,2});

        if (choice == 1)
        {
            vector<int> spaces;
            for (int t : im->getMistTokens()) if (t != -1 && bord.isEmpty(t)) spaces.push_back(t);

            if (!spaces.empty())
            {
                effectLog << "Choose a fog token space: ";
                for (int t : spaces) effectLog << "   " << t;
                int dest = getChoice(spaces);

                bord.deletCharacter(im->getSpace());
                bord.addCharacter(dest , im);
            }
        }
        else
        {
            vector<int> tokens;
            for (int t : im->getMistTokens()) if (t != -1) tokens.push_back(t);

            if (!tokens.empty())
            {
                effectLog << "Choose a fog token to move: ";
                for (int t : tokens) effectLog << "   " << t;
                int tokenPos = getChoice(tokens);

                vector<int> emptySpaces;
                for (int i = 0; i < 32; i++)
                    if (bord.isEmpty(i) && !im->isMistPosition(i))
                        emptySpaces.push_back(i);

                effectLog << "\nMove it to: ";
                for (int s : emptySpaces) effectLog << "   " << s;
                int dest = getChoice(emptySpaces);

                for (int i = 0; i < 3; i++)
                    if (im->getMistToken(i) == tokenPos) { im->setMistToken(i , dest); break; }
            }
        }

        return;

    }

    else if (card.getName() == "Slip Away")
    {
        if (cancelEffectIM)
        {
            effectLog << "Slip Away effect was canceled.\n";
            return;
        }

        effectLog << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(attacker);
        if (im == nullptr) return;

        vector<int> tokens;
        for (int t : im->getMistTokens()) if (t != -1) tokens.push_back(t);

        if (tokens.empty())
        {
            effectLog << "No fog tokens on the board.\n";
            return;
        }

        effectLog << "Choose a fog token to move: ";
        for (int t : tokens) effectLog << "   " << t;
        int tokenPos = getChoice(tokens);

        vector<int> emptySpaces;
        for (int i = 0; i < 32; i++)
        {
            if (bord.isEmpty(i) && !im->isMistPosition(i))
                emptySpaces.push_back(i);
        }

        effectLog << "\nChoose an empty space for the fog token: ";
        for (int s : emptySpaces) effectLog << "   " << s;
        int dest = getChoice(emptySpaces);

        for (int i = 0; i < 3; i++)
        {
            if (im->getMistToken(i) == tokenPos) { im->setMistToken(i , dest); break; }
        }

        bord.deletCharacter(im->getSpace());
        bord.addCharacter(dest , im);

        effectLog << "The fog token moved to " << dest << " and Invisible Man moved there with it.\n";

        return;

    }

}    


void Controller::applyEffectScheme(Card& card ,Player* self, Player* opponent , Character* attacker )
{
    GuiEffectLogger effectLog([this](const std::string& message) { guiLog(message); });
    activeDecider = self;

//<<<<<<<<<<<<<<<<< Dracula 

    if (card.getName() == "MistForm")
    {
        if (cancelEffectDR)
        {
            effectLog << "MistForm effect was canceled.\n";
            return;
        }

        effectLog << "\nEffect >> " << card.geteffect() << endl;

        int pos;
        while(true)
        {
            effectLog << "Choose a space for Dracula: ";
            pos = getInt();
            if(pos >= 0 && pos <= 31 && bord.isEmpty(pos))
            {
                bord.deletCharacter(self->getHero()->getSpace());
                bord.addCharacter(pos , self->getHero());
                break;
            }
            effectLog << "Invaliad input. Plase try again." << endl;
        }
        effectLog << "Dracula was placed\n" ;
        effectLog << "You have gained an extra action.\n";

        gamerand--;

        return;

    }

    else if (card.getName() == "Baptism of Blood")
    {
        if (cancelEffectDR)
        {
            effectLog << "Baptism of Blood effect was canceled.\n";
            return;
        }

        effectLog << "\nEffect >> " << card.geteffect() << endl;

        self->getHero()->heal(2);
        effectLog << "Dracula recovered 2 health.\n";

        vector<int> heroZones = bord.getCharacterZone(self->getHero());

        for(int i = 1 ; i < self->getfighterCount() ; i++)
        {
            Character* sister = self->getFighter(i);
            if(sister == nullptr || sister->checkalive())
                continue;

            vector<int> spaces = bord.getEmptyZone(heroZones);
            if(spaces.empty())
            {
                effectLog << "\nNo available space to place " << sister->getName() << ".\n";
                continue;
            }

            effectLog << "\nAvailable spaces for " << sister->getName() << ":  ";
            for(int s : spaces)
            {
                effectLog << s << "   ";
            }
            int pos = getChoice(spaces);
            sister->setSpace(pos);
            sister->heal(1);
            bord.addCharacter(pos , sister);
            effectLog << sister->getName() << " placed on " << pos << ".\n";
        }

        return;

    }

    else if (card.getName() == "Prey Upon")
    {
        if (cancelEffectDR)
        {
            effectLog << "Prey Upon effect was canceled.\n";
            return;
        }

        effectLog << "\nEffect >> " << card.geteffect() << endl;

        int amount = 0;
        vector<int> draculaAdjence = bord.getCharacterAdjacent(attacker);
        for(int adjen : draculaAdjence)
        {
            if(!bord.isEmpty(adjen))
            {
                if(bord.getCharacter(adjen)->getowner() != attacker->getowner())
                {
                    bord.getCharacter(adjen)->takeDamage(1);
                    effectLog << bord.getCharacter(adjen)->getName() << "took 1 damage\n";
                    amount++;
                }
            }
        }
        attacker->heal(amount);
        effectLog << attacker->getName() << " gained " << amount << " health.\n";
        
        return;

    }
    
    else if (card.getName() == "Ravening Seduction")
    {
        if (cancelEffectDR)
        {
            effectLog << "Ravening Seduction effect was canceled.\n";
            return;
        }

        effectLog << "\nEffect >> " << card.geteffect() << endl;

        int choose = 0;
        vector<Character*> choices;
        vector<int> valid;
        int number = 1;
    
        for (Character* ch : self->getCharacters())
        {
            if(ch->checkalive())
            {
                effectLog << number << "." << ch->getName() << endl;
                choices.push_back(ch);
                valid.push_back(number);
                number++;
            }
        }
        for (Character* ch : opponent->getCharacters())
        {
            if(ch->checkalive())
            {
                effectLog << number << "." << ch->getName() << endl;
                choices.push_back(ch);
                valid.push_back(number);
                number++;
            }
        }
        effectLog << "Choose a character to move: ";
        choose = getChoice(valid);
        Character* selected = choices[choose - 1];

        move(2 , selected);

        vector<int> target = bord.getCharacterAdjacent(selected);
        number = 0;
        for(int adjenc : target)
        {
            if(bord.isEmpty(adjenc))
                continue;

            Character* occupant = bord.getCharacter(adjenc);
            for(int i = 1 ; i < self->getfighterCount() ; i++)
            {
                Character* sister = self->getFighter(i);
                if(sister != nullptr && sister->checkalive() && occupant == sister)
                {
                    number++;
                }
            }
        }

        selected->takeDamage(number);
        if(number > 0)
        {
            effectLog << selected->getName() << "took " << number << " damage." << endl;
        }
        else
        {
            effectLog << selected->getName() << "took no damage." << endl;
        }

        return;

    }

//<<<<<<<<<<<<<<<<< sherlok

    else if (card.getName() == "Administer Aid")
    {
        
        effectLog << "\nEffect >> " << card.geteffect() << endl; 

        Character* holmes = self->getHero();
        Character* watson = self->getFighter(1);

        if (watson != nullptr && watson->checkalive())
        {
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
                if (watson->getSpace() != -1)
                {
                    bord.deletCharacter(watson->getSpace());
                }

                bord.addCharacter(emptyAdjacent[0] , watson);
                effectLog << "Watson placed adjacent to Holmes.\n";
            }
            else
            {
                effectLog << "No empty space adjacent to Holmes for Watson.\n";
            }
        }
        else
        {
            effectLog << "Watson is not available.\n";
        }

        holmes->heal(1);
        effectLog << "Holmes healed 1 HP \n";
        
        try
        {
            self->getDeck()->draw();
            effectLog << "added to " << self->getName() << " hand\n\n";
        }
        catch(const runtime_error& e)
        {
            effectLog << e.what() << endl;
            damageAllFighters(self, 2);
            effectLog << "All character on team took 2 damage";
        }
        
        return;

    }
    
    else if (card.getName() == "Confirm Suspicion")
    {
        effectLog << "\nEffect >> " << card.geteffect() << endl; 
        
        int choose = 0;
        vector<Character*> choices;
        vector<int> valid;
        int k = 1;

        for (Character* ch : opponent->getCharacters())
        {
            if(ch->checkalive())
            {
                effectLog << k << "." << ch->getName() << endl;
                choices.push_back(ch);
                valid.push_back(k);
                k++;
            }
        }
        effectLog << "Choose an opponent: ";
        choose = getChoice(valid);
        Character* Rival = choices[choose - 1];

        effectLog << "Choose a number: ";
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
            effectLog << "The opponent has no card with an attack or defense value of " << number << ".\n";

            return;
        }
        
        Card burn;
        activeDecider = opponent;
        while (true)
        {
            effectLog << opponent->getName() << ", choose a card with " << number << "attack or defense: ";
            int select = getInt();
            if(select > 0 && select < (opponent->getDeck()->gethandSize() + 1))
            {
                if(number == opponent->getDeck()->getHandcard(select - 1).getAttack())
                {
                    burn = opponent->getDeck()->playCard(select - 1 , burn);
                    break;
                }
            }
            effectLog << "Invalid input.\n";
        }
        activeDecider = self;
        
        
        int burnBoost = burn.getBoost();

        Rival->takeDamage(burnBoost);

        effectLog << Rival->getName() << " took " << burnBoost << " damamge" << endl;

        return;

    }

    else if (card.getName() == "Eliminate the Impossible")
    {
        effectLog << "\nEffect >> " << card.geteffect() << endl; 

        int index;
        while (true)
        {
            effectLog << "Choose a card to burn: ";
            index = getInt();
            if(index > 0 && index < (opponent->getDeck()->gethandSize() + 1))
            {
                break;
            }
            effectLog << "invalid input.";
        }
        

        Card burned;
        burned = opponent->getDeck()->playCard(index - 1 , burned);
        effectLog << burned.getName() <<  " was butn" << endl;

        return;

    }

    else if (card.getName() == "Master of Disguise")
    {
        effectLog << "\nEffect >> " << card.geteffect() << endl; 

        Character* holmes = self->getHero();

        int choose = 0;
        vector<Character*> choices;
        vector<int> valid;
        int number = 1;

        for (Character* ch : opponent->getCharacters())
        {
            if(ch->checkalive())
            {
                effectLog << number << "." << ch->getName() << endl;
                choices.push_back(ch);
                valid.push_back(number);
                number++;
            }
        }
        effectLog << "Choose a character to swap positions with: ";
        choose = getChoice(valid);
        Character* Rival = choices[choose - 1];

        int holmespos = holmes->getSpace();
        int enemypos = Rival->getSpace();

        bord.deletCharacter(holmespos);
        bord.deletCharacter(enemypos);

        bord.addCharacter(enemypos , holmes);
        bord.addCharacter(holmespos , Rival);

        Rival->takeDamage(1);
        effectLog << Rival->getName() << " took a damage." << endl;
        return;
        
    }

//<<<<<<<<<<<<<<<<< Invisible Man

    else if (card.getName() == "Reign of Terror")
    {
        if (cancelEffectIM)
        {
            effectLog << "Reign of Terror effect was canceled.\n";
            return;
        }

        effectLog << "\nEffect >> " << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(attacker);
        if (im == nullptr || !im->isMistPosition(im->getSpace()))
        {
            effectLog << "Invisible Man is not on a fog token.\n";
            return;
        }

        int choose = 0;
        vector<Character*> choices;
        vector<int> valid;
        int number = 1;

        for (Character* ch : opponent->getCharacters())
        {
            if (ch->checkalive())
            {
                effectLog << number << "." << ch->getName() << endl;
                choices.push_back(ch);
                valid.push_back(number);
                number++;
            }
        }

        if (choices.empty())
        {
            effectLog << "No opposing fighters.\n";
            return;
        }

        effectLog << "Choose a target: ";
        choose = getChoice(valid);
        Character* target = choices[choose - 1];

        target->takeDamage(2);
        effectLog << target->getName() << " took 2 damage.\n";

        if (!target->checkalive() && target->getSpace() != -1)
        {
            bord.deletCharacter(target->getSpace());
            target->setSpace(-1);
        }

        return;

    }

    else if (card.getName() == "Rolling Fog")
    {
        if (cancelEffectIM)
        {
            effectLog << "Rolling Fog effect was canceled.\n";
            return;
        }

        effectLog << "\nEffect >> " << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(attacker);
        if (im == nullptr) return;

        vector<int> tokens;
        for (int t : im->getMistTokens()) if (t != -1) tokens.push_back(t);

        if (!tokens.empty())
        {
            effectLog << "Choose a fog token to move: ";
            for (int t : tokens) effectLog << "   " << t;
            int tokenPos = getChoice(tokens);

            vector<int> emptySpaces;
            for (int i = 0; i < 32; i++)
                if (bord.isEmpty(i) && !im->isMistPosition(i))
                    emptySpaces.push_back(i);

            effectLog << "\nMove it to: ";
            for (int s : emptySpaces) effectLog << "   " << s;
            int dest = getChoice(emptySpaces);

            for (int i = 0; i < 3; i++)
                if (im->getMistToken(i) == tokenPos) { im->setMistToken(i , dest); break; }
        }

        effectLog << "You gained an extra action.\n";
        gamerand--;

        return;

    }

    else if (card.getName() == "Step Lightly")
    {
        if (cancelEffectIM)
        {
            effectLog << "Step Lightly effect was canceled.\n";
            return;
        }

        effectLog << "\nEffect >> " << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(attacker);

        vector<int> adjacent = bord.getCharacterAdjacent(attacker);
        Character* target = nullptr;

        for (int pos : adjacent)
        {
            if (!bord.isEmpty(pos) && bord.getCharacter(pos)->getowner() != attacker->getowner())
            {
                target = bord.getCharacter(pos);
                break;
            }
        }

        if (target == nullptr)
        {
            effectLog << "No adjacent fighter to hit.\n";
            return;
        }

        int damage = (im != nullptr && im->isMistPosition(im->getSpace())) ? 3 : 1;
        target->takeDamage(damage);
        effectLog << target->getName() << " took " << damage << " damage.\n";

        if (!target->checkalive() && target->getSpace() != -1)
        {
            bord.deletCharacter(target->getSpace());
            target->setSpace(-1);
        }

        if (im != nullptr)
        {
            vector<int> tokens;
            for (int t : im->getMistTokens()) if (t != -1) tokens.push_back(t);

            if (!tokens.empty())
            {
                activeDecider = opponent;

                effectLog << opponent->getName() << ", choose a fog token to move: ";
                for (int t : tokens) effectLog << "   " << t;
                int tokenPos = getChoice(tokens);

                vector<int> emptySpaces;
                for (int i = 0; i < 32; i++)
                    if (bord.isEmpty(i) && !im->isMistPosition(i))
                        emptySpaces.push_back(i);

                effectLog << "\nMove it to: ";
                for (int s : emptySpaces) effectLog << "   " << s;
                int dest = getChoice(emptySpaces);

                for (int i = 0; i < 3; i++)
                    if (im->getMistToken(i) == tokenPos) { im->setMistToken(i , dest); break; }

                activeDecider = self;
            }
        }

        return;

    }

    else if (card.getName() == "Vanish")
    {
        if (cancelEffectIM)
        {
            effectLog << "Vanish effect was canceled.\n";
            return;
        }

        effectLog << "\nEffect >> " << card.geteffect() << endl;

        attacker->heal(1);
        effectLog << attacker->getName() << " recovered 1 health.\n";

        bord.deletCharacter(attacker->getSpace());
        attacker->setSpace(-1);

        effectLog << "Invisible Man vanishes from the board.\n";

        int pos;
        while(true)
        {
            effectLog << "Choose a space for Invisible Man to reappear: ";
            pos = getInt();
            if (pos >= 0 && pos < 32 && bord.isEmpty(pos))
                break;
            effectLog << "Invalid space. Try again.\n";
        }

        bord.addCharacter(pos , attacker);
        effectLog << "Invisible Man reappeared at " << pos << ".\n";

        return;

    }

}