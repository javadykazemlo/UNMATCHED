#include <iostream>
#include <string>
#include <vector>
#include "Dracula.hpp"

using namespace std;

Dracula::Dracula(int owner): Character("Dracula", 13, 2, 0 , owner , true)
{
    
}


void Dracula::ability(Bord bord , Player* player)
{
    
    cout << "\nDo you want to use Dracula's ability? (y/n): ";
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
            cout << "No adjacent fighters to attack!\n";
            return;
        }

        for (int i = 0; i < targets.size(); i++)
        {
            cout << i + 1 << ". " << targets[i]->getName() << endl;
        }
        
        while(true)
        {
            int k;
            cout << "Select target to damamge: ";
            cin >> k;
        
            if (k >= 1 && k <= targets.size())
            {
                targets[k - 1]->takeDamage(0, 1);
                try
                {
                    player->getDeck()->draw();
                    cout << "1 card added to " << player->getName() << " hand\n\n";
                }
                catch(const runtime_error& e)
                {
                    cout << e.what() << endl;
                    for(int i = 0 ; i <  player->getfighterCount() ; i++)
                    {
                        player->getsidekick(i)->takeDamage(2,0);
                    }
                    cout << "All character on team took 2 damage";
                }
                cout << "Ability used! 1 damage dealt.\n";
                return;
            }
            else
            {
                cout << "Invalid choice!\n";
            }
        }
    }
    else
    {
        cout << "Not using ability.\n";
        return;
    }
    
}

Dracula::~Dracula()
{
}
