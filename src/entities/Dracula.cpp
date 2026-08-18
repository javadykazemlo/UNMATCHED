#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "Dracula.hpp"
#include "core/GuiEffectLogger.hpp"

using namespace std;

Dracula::Dracula(int owner): Character("Dracula", 13, 2, 0 , owner , true)
{
    
}


void Dracula::ability(Bord& bord , Player* player)
{
    GuiEffectLogger effectLog([](const std::string& message)
    {
        std::clog << message << '\n';
    });

    // An AI-controlled Dracula must never block on cin (there is no console
    // user to answer for it, and in guiMode this call runs on the AI turn's
    // background thread). Decide with a simple heuristic instead: use the
    // ability whenever there is an adjacent living fighter to hit.
    if (player != nullptr && player->isAI())
    {
        vector<int> adjacent = bord.getCharacterAdjacent(this);
        vector<Character*> targets;
        for (int pos : adjacent)
        {
            Character* occupant = bord.getCharacter(pos);
            if (occupant != nullptr && occupant->checkalive())
                targets.push_back(occupant);
        }

        if (targets.empty())
        {
            effectLog << "AI: no adjacent fighters to attack, skipping ability.\n";
            return;
        }

        Character* target = targets[static_cast<std::size_t>(rand()) % targets.size()];
        target->takeDamage(1);
        effectLog << "AI: Dracula's ability dealt 1 damage to " << target->getName() << ".\n";

        if (!target->checkalive())
        {
            bord.deletCharacter(target->getSpace());
            target->setSpace(-1);
            effectLog << target->getName() << " was defeated.\n";
        }

        try
        {
            player->getDeck()->draw();
            effectLog << "1 card added to " << player->getName() << " hand\n\n";
        }
        catch(const runtime_error& e)
        {
            effectLog << e.what() << endl;
            for(Character* fighter : player->getCharacters())
            {
                if(fighter && fighter->checkalive())
                    fighter->takeDamage(2);
            }
            effectLog << "All character on team took 2 damage";
        }
        effectLog << "Ability used! 1 damage dealt.\n";
        return;
    }

    effectLog << "\nDo you want to use Dracula's ability? (y/n): ";
    char choice;
    cin >> choice;  

    if (choice == 'y' || choice == 'Y')
    {
        vector<int> adjacent = bord.getCharacterAdjacent(this);
        vector<Character*> targets;

        for (int pos : adjacent)
        {
            Character* occupant = bord.getCharacter(pos);
            if (occupant != nullptr && occupant->checkalive())
            {
                targets.push_back(occupant);
            }
        }

        if (targets.empty())
        {
            effectLog << "No adjacent fighters to attack!\n";
            return;
        }

        for (int i = 0; i < targets.size(); i++)
        {
            effectLog << i + 1 << ". " << targets[i]->getName() << endl;
        }
        
        while(true)
        {
            int k;
            effectLog << "Select target to damamge: ";
            cin >> k;
        
            if (k >= 1 && k <= targets.size())
            {
                Character* target = targets[k - 1];
                target->takeDamage(1);
                effectLog << "1 damage dealt to " << target->getName() << ".\n";

                if (!target->checkalive())
                {
                    bord.deletCharacter(target->getSpace());
                    target->setSpace(-1);
                    effectLog << target->getName() << " was defeated.\n";
                }

                try
                {
                    player->getDeck()->draw();
                    effectLog << "1 card added to " << player->getName() << " hand\n\n";
                }
                catch(const runtime_error& e)
                {
                    effectLog << e.what() << endl;
                    for(Character* fighter : player->getCharacters())
                    {
                        if(fighter && fighter->checkalive())
                            fighter->takeDamage(2);
                    }
                    effectLog << "All character on team took 2 damage";
                }
                effectLog << "Ability used! 1 damage dealt.\n";
                return;
            }
            else
            {
                effectLog << "Invalid choice!\n";
            }
        }
    }
    else
    {
        effectLog << "Not using ability.\n";
        return;
    }
    
}