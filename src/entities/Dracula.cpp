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

    effectLog << "\nDo you want to use Dracula's ability? (y/n): ";
    char choice;
    cin >> choice;  

    if (choice == 'y' || choice == 'Y')
    {
        vector<int> adjacent = bord.getCharacterAdjacent(this);
        vector<Character*> targets;

        for (int pos : adjacent)
        {
            if (bord.getCharacter(pos) != nullptr)
            {
                targets.push_back(bord.getCharacter(pos));
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
                targets[k - 1]->takeDamage(1);
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