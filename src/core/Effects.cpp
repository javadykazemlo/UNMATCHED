#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <streambuf>
#include "core/Controller.hpp"
#include "entities/invisible_man.hpp"

using namespace std;


namespace
{
class EffectStreamBuf final : public std::streambuf
{
    Controller& controller;
    bool gui;
public:
    EffectStreamBuf(Controller& c, bool useGui) : controller(c), gui(useGui) {}
protected:
    std::streamsize xsputn(const char* s, std::streamsize n) override
    {
        if (gui) controller.guiLogEffect(std::string(s, static_cast<std::size_t>(n)));
        else std::cout.write(s, n);
        return n;
    }
    int_type overflow(int_type ch) override
    {
        if (ch != traits_type::eof())
        {
            char c = traits_type::to_char_type(ch);
            if (gui) controller.guiLogEffect(std::string(1, c));
            else std::cout.put(c);
        }
        return ch;
    }
    int sync() override
    {
        if (!gui) std::cout.flush();
        return 0;
    }
};

class EffectOStream final : public std::ostream
{
    EffectStreamBuf buffer;
public:
    EffectOStream(Controller& c, bool useGui) : std::ostream(&buffer), buffer(c, useGui) {}
};
}


void Controller::applyEffect(Card& card , Card& enemycard ,Player* self, Player* opponent , Character* attacker , Character* defender , bool woncombat)
{
    activeDecider = self;
    EffectOStream effectCout(*this, guiMode);
#define cout effectCout

//<<<<<<<<<<<<<<<<< Dracula

    if (card.getName() == "Feeding Frenzy")
    {
        if (cancelEffectDR)
        {
            cout << "Feeding Frenzy effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

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
            if(getYesNo())
            {
                int handSize = self->getDeck()->gethandSize();
    
                int choice;
                while (true)
                {
                    cout << "choice a card: ";
                    choice = getInt();
                    if(choice > 0 && choice <= handSize)
                    {
                        break;
                    }
                    cout << "Invalid input.";
                }

                Card selected;
                selected = self->getDeck()->playCard(choice - 1 , selected);
    
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
            damageAllFighters(self, 2);
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

        string oppName = opponent->getHero()->getName();
        if (oppName == "sherlock") cancelEffectSH = true;
        else if (oppName == "invisible man") cancelEffectIM = true;
    
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
                damageAllFighters(opponent, 2);
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
                damageAllFighters(self, 2);
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

        string oppName = opponent->getHero()->getName();
        if (oppName == "Dracula") cancelEffectDR = true;
        else if (oppName == "invisible man") cancelEffectIM = true;
        
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

//<<<<<<<<<<<<<<<<< Invisible Man

    else if (card.getName() == "Coded Notes")
    {
        if (cancelEffectIM)
        {
            cout << "Coded Notes effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        try
        {
            self->getDeck()->draw(3);
            cout << "3 cards added to " << self->getName() << "'s hand.\n";
        }
        catch(const runtime_error& e)
        {
            cout << e.what() << endl;
            damageAllFighters(self, 2);
            cout << "All character on team took 2 damage";
        }

        int handSize = self->getDeck()->gethandSize();
        cout << "Choose the first card to put on top of your deck: ";
        int first = getInt();
        while(first <= 0 || first > handSize)
        {
            cout << "Invalid card number. Try again: ";
            first = getInt();
        }
        Card firstCard = self->getDeck()->removeFromHand(first - 1);

        handSize = self->getDeck()->gethandSize();
        cout << "Choose the second card to put on top of your deck: ";
        int second = getInt();
        while(second <= 0 || second > handSize)
        {
            cout << "Invalid card number. Try again: ";
            second = getInt();
        }
        Card secondCard = self->getDeck()->removeFromHand(second - 1);

        self->getDeck()->putOnTop(secondCard);
        self->getDeck()->putOnTop(firstCard);

        cout << firstCard.getName() << " and " << secondCard.getName() << " were placed on top of your deck.\n";

        return;

    }

    else if (card.getName() == "Confound")
    {
        if (cancelEffectIM)
        {
            cout << "Confound effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        cout << opponent->getName() << ", do you want to discard 1 card? (y/n): ";
        activeDecider = opponent;
        bool willDiscard = getYesNo();

        if (willDiscard)
        {
            int handSize = opponent->getDeck()->gethandSize();

            cout << "Choose a card to discard: ";
            int idx = getInt();
            while(idx <= 0 || idx > handSize)
            {
                cout << "Invalid card number. Try again: ";
                idx = getInt();
            }

            Card discarded;
            discarded = opponent->getDeck()->playCard(idx - 1 , discarded);
            cout << discarded.getName() << " was discarded.\n";
            activeDecider = self;
        }
        else
        {
            activeDecider = self;
            invisible_man* im = dynamic_cast<invisible_man*>(self->getHero());
            if (im != nullptr)
            {
                cout << "You may move each fog token to any other space.\n";
                for (int i = 0; i < 3; i++)
                {
                    int tokenPos = im->getMistToken(i);
                    if (tokenPos == -1) continue;

                    vector<int> options;
                    for (int j = 0; j < 32; j++)
                        if (bord.isEmpty(j) && !im->isMistPosition(j))
                            options.push_back(j);
                    options.push_back(tokenPos);

                    cout << "\nMove fog token at " << tokenPos << " to (or keep at " << tokenPos << "): ";
                    for (int s : options) cout << "   " << s;
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
            cout << "Covert Preparation effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        try
        {
            self->getDeck()->draw();
            cout << "1 card added to " << self->getName() << "'s hand.\n";
        }
        catch(const runtime_error& e)
        {
            cout << e.what() << endl;
            damageAllFighters(self, 2);
            cout << "All character on team took 2 damage";
        }

        invisible_man* im = dynamic_cast<invisible_man*>(self->getHero());
        if (im == nullptr) return;

        vector<int> tokens;
        for (int t : im->getMistTokens()) if (t != -1) tokens.push_back(t);

        if (tokens.size() >= 1)
        {
            cout << "Choose a fog token to move: ";
            for (int t : tokens) cout << "   " << t;
            int tokenPos = getChoice(tokens);

            vector<int> emptySpaces;
            for (int i = 0; i < 32; i++)
                if (bord.isEmpty(i) && !im->isMistPosition(i))
                    emptySpaces.push_back(i);

            cout << "\nMove it to: ";
            for (int s : emptySpaces) cout << "   " << s;
            int dest = getChoice(emptySpaces);

            for (int i = 0; i < 3; i++)
                if (im->getMistToken(i) == tokenPos) { im->setMistToken(i , dest); break; }
        }

        if (tokens.size() >= 2)
        {
            activeDecider = opponent;

            cout << "\n" << opponent->getName() << ", choose a different fog token to move: ";
            for (int t : tokens) cout << "   " << t;
            int tokenPos2 = getChoice(tokens);

            vector<int> emptySpaces2;
            for (int i = 0; i < 32; i++)
                if (bord.isEmpty(i) && !im->isMistPosition(i))
                    emptySpaces2.push_back(i);

            cout << "\nMove it to: ";
            for (int s : emptySpaces2) cout << "   " << s;
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
            cout << "Dreaming of Revenge effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(self->getHero());
        if (im == nullptr || !im->isMistPosition(im->getSpace()))
        {
            cout << "Invisible Man is not on a fog token.\n";
            return;
        }

        int amount = 0;
        for (Character* ch : opponent->getCharacters())
        {
            if (ch->checkalive() && im->isMistPosition(ch->getSpace()))
            {
                ch->takeDamage(1);
                cout << ch->getName() << " took 1 damage.\n";

                if (!ch->checkalive() && ch->getSpace() != -1)
                {
                    bord.deletCharacter(ch->getSpace());
                    ch->setSpace(-1);
                }
                amount++;
            }
        }

        if (amount == 0)
            cout << "No opposing fighters are on fog tokens.\n";

        return;

    }

    else if (card.getName() == "Emerge from Mist")
    {
        if (cancelEffectIM)
        {
            cout << "Emerge from Mist effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(attacker);
        if (im != nullptr && im->isMistPosition(im->getSpace()))
        {
            card.setAttack(5);
            cout << "This card's value becomes 5.\n";
        }

        return;

    }

    else if (card.getName() == "Impossible to See")
    {
        cout << card.geteffect() << endl;

        enemycard.setAttack(0);

        string oppName = opponent->getHero()->getName();

        if (oppName == "Dracula") cancelEffectDR = true;
        else if (oppName == "sherlock") cancelEffectSH = true;
        else if (oppName == "invisible man") cancelEffectIM = true;

        cout << "The opponent's card value is now 0 and cannot be changed by card effects.\n";

        return;

    }

    else if (card.getName() == "Into Thin Air")
    {
        if (cancelEffectIM)
        {
            cout << "Into Thin Air effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(defender);
        if (im == nullptr) return;

        move(1 , im);

        vector<int> tokens;
        for (int t : im->getMistTokens()) if (t != -1) tokens.push_back(t);

        if (!tokens.empty())
        {
            activeDecider = opponent;

            cout << "\n" << opponent->getName() << ", choose a fog token to move: ";
            for (int t : tokens) cout << "   " << t;
            int tokenPos = getChoice(tokens);

            vector<int> emptySpaces;
            for (int i = 0; i < 32; i++)
            {
                if (bord.isEmpty(i) && !im->isMistPosition(i))
                    emptySpaces.push_back(i);
            }

            cout << "\nMove it to: ";
            for (int s : emptySpaces) cout << "   " << s;

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
            cout << "Lurking effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        try
        {
            self->getDeck()->draw();
            cout << "1 card added to " << self->getName() << "'s hand.\n";
        }
        catch(const runtime_error& e)
        {
            cout << e.what() << endl;
            damageAllFighters(self, 2);
            cout << "All character on team took 2 damage";
        }

        invisible_man* im = dynamic_cast<invisible_man*>(defender);
        if (im == nullptr) return;

        cout << "\nChoose an effect:\n 1. Move Invisible Man to a fog token space\n 2. Move 1 fog token up to 3 spaces\nChoice: ";
        int choice = getChoice({1,2});

        if (choice == 1)
        {
            vector<int> spaces;
            for (int t : im->getMistTokens()) if (t != -1 && bord.isEmpty(t)) spaces.push_back(t);

            if (!spaces.empty())
            {
                cout << "Choose a fog token space: ";
                for (int t : spaces) cout << "   " << t;
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
                cout << "Choose a fog token to move: ";
                for (int t : tokens) cout << "   " << t;
                int tokenPos = getChoice(tokens);

                vector<int> emptySpaces;
                for (int i = 0; i < 32; i++)
                    if (bord.isEmpty(i) && !im->isMistPosition(i))
                        emptySpaces.push_back(i);

                cout << "\nMove it to: ";
                for (int s : emptySpaces) cout << "   " << s;
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
            cout << "Slip Away effect was canceled.\n";
            return;
        }

        cout << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(attacker);
        if (im == nullptr) return;

        vector<int> tokens;
        for (int t : im->getMistTokens()) if (t != -1) tokens.push_back(t);

        if (tokens.empty())
        {
            cout << "No fog tokens on the board.\n";
            return;
        }

        cout << "Choose a fog token to move: ";
        for (int t : tokens) cout << "   " << t;
        int tokenPos = getChoice(tokens);

        vector<int> emptySpaces;
        for (int i = 0; i < 32; i++)
        {
            if (bord.isEmpty(i) && !im->isMistPosition(i))
                emptySpaces.push_back(i);
        }

        cout << "\nChoose an empty space for the fog token: ";
        for (int s : emptySpaces) cout << "   " << s;
        int dest = getChoice(emptySpaces);

        for (int i = 0; i < 3; i++)
        {
            if (im->getMistToken(i) == tokenPos) { im->setMistToken(i , dest); break; }
        }

        bord.deletCharacter(im->getSpace());
        bord.addCharacter(dest , im);

        cout << "The fog token moved to " << dest << " and Invisible Man moved there with it.\n";

        return;

    }

}    


void Controller::applyEffectScheme(Card& card ,Player* self, Player* opponent , Character* attacker )
{
    activeDecider = self;
    EffectOStream effectCout(*this, guiMode);
#define cout effectCout

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
            if(pos >= 0 && pos <= 31 && bord.isEmpty(pos))
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

        vector<int> heroZones = bord.getCharacterZone(self->getHero());

        for(int i = 1 ; i < self->getfighterCount() ; i++)
        {
            Character* sister = self->getFighter(i);
            if(sister == nullptr || sister->checkalive())
                continue;

            vector<int> spaces = bord.getEmptyZone(heroZones);
            if(spaces.empty())
            {
                cout << "\nNo available space to place " << sister->getName() << ".\n";
                continue;
            }

            cout << "\nAvailable spaces for " << sister->getName() << ":  ";
            for(int s : spaces)
            {
                cout << s << "   ";
            }
            int pos = getChoice(spaces);
            sister->setSpace(pos);
            sister->heal(1);
            bord.addCharacter(pos , sister);
            cout << sister->getName() << " placed on " << pos << ".\n";
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
                cout << "Watson placed adjacent to Holmes.\n";
            }
            else
            {
                cout << "No empty space adjacent to Holmes for Watson.\n";
            }
        }
        else
        {
            cout << "Watson is not available.\n";
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
            damageAllFighters(self, 2);
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
        choose = getChoice(valid);
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

            return;
        }
        
        Card burn;
        activeDecider = opponent;
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
        activeDecider = self;
        
        
        int burnBoost = burn.getBoost();

        Rival->takeDamage(burnBoost);

        cout << Rival->getName() << " took " << burnBoost << " damamge" << endl;

        return;

    }

    else if (card.getName() == "Eliminate the Impossible")
    {
        cout << "\nEffect >> " << card.geteffect() << endl; 

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
        choose = getChoice(valid);
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

//<<<<<<<<<<<<<<<<< Invisible Man

    else if (card.getName() == "Reign of Terror")
    {
        if (cancelEffectIM)
        {
            cout << "Reign of Terror effect was canceled.\n";
            return;
        }

        cout << "\nEffect >> " << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(attacker);
        if (im == nullptr || !im->isMistPosition(im->getSpace()))
        {
            cout << "Invisible Man is not on a fog token.\n";
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
                cout << number << "." << ch->getName() << endl;
                choices.push_back(ch);
                valid.push_back(number);
                number++;
            }
        }

        if (choices.empty())
        {
            cout << "No opposing fighters.\n";
            return;
        }

        cout << "Choose a target: ";
        choose = getChoice(valid);
        Character* target = choices[choose - 1];

        target->takeDamage(2);
        cout << target->getName() << " took 2 damage.\n";

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
            cout << "Rolling Fog effect was canceled.\n";
            return;
        }

        cout << "\nEffect >> " << card.geteffect() << endl;

        invisible_man* im = dynamic_cast<invisible_man*>(attacker);
        if (im == nullptr) return;

        vector<int> tokens;
        for (int t : im->getMistTokens()) if (t != -1) tokens.push_back(t);

        if (!tokens.empty())
        {
            cout << "Choose a fog token to move: ";
            for (int t : tokens) cout << "   " << t;
            int tokenPos = getChoice(tokens);

            vector<int> emptySpaces;
            for (int i = 0; i < 32; i++)
                if (bord.isEmpty(i) && !im->isMistPosition(i))
                    emptySpaces.push_back(i);

            cout << "\nMove it to: ";
            for (int s : emptySpaces) cout << "   " << s;
            int dest = getChoice(emptySpaces);

            for (int i = 0; i < 3; i++)
                if (im->getMistToken(i) == tokenPos) { im->setMistToken(i , dest); break; }
        }

        cout << "You gained an extra action.\n";
        gamerand--;

        return;

    }

    else if (card.getName() == "Step Lightly")
    {
        if (cancelEffectIM)
        {
            cout << "Step Lightly effect was canceled.\n";
            return;
        }

        cout << "\nEffect >> " << card.geteffect() << endl;

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
            cout << "No adjacent fighter to hit.\n";
            return;
        }

        int damage = (im != nullptr && im->isMistPosition(im->getSpace())) ? 3 : 1;
        target->takeDamage(damage);
        cout << target->getName() << " took " << damage << " damage.\n";

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

                cout << opponent->getName() << ", choose a fog token to move: ";
                for (int t : tokens) cout << "   " << t;
                int tokenPos = getChoice(tokens);

                vector<int> emptySpaces;
                for (int i = 0; i < 32; i++)
                    if (bord.isEmpty(i) && !im->isMistPosition(i))
                        emptySpaces.push_back(i);

                cout << "\nMove it to: ";
                for (int s : emptySpaces) cout << "   " << s;
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
            cout << "Vanish effect was canceled.\n";
            return;
        }

        cout << "\nEffect >> " << card.geteffect() << endl;

        attacker->heal(1);
        cout << attacker->getName() << " recovered 1 health.\n";

        bord.deletCharacter(attacker->getSpace());
        attacker->setSpace(-1);

        cout << "Invisible Man vanishes from the board.\n";

        int pos;
        while(true)
        {
            cout << "Choose a space for Invisible Man to reappear: ";
            pos = getInt();
            if (pos >= 0 && pos < 32 && bord.isEmpty(pos))
                break;
            cout << "Invalid space. Try again.\n";
        }

        bord.addCharacter(pos , attacker);
        cout << "Invisible Man reappeared at " << pos << ".\n";

        return;

    }

#undef cout
}