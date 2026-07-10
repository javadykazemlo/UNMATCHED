#include <iostream>
#include <string>
#include <vector>
#include "Dracula.hpp"

using namespace std;

Dracula::Dracula(int owner): Character("Dracula", 13, 2, 0 , owner)
{
    
}


void Dracula::ability(Bord* bord , Player* player)
{

    cout << "Do you want to use Dracula's ability? (y/n): ";
    char choice;
    cin >> choice;  

    if (choice == 'y' || choice == 'Y')
    {
        vector<int> adjacent = bord->getAdjacentCharacter(this);
        vector<Character*> targets;

        for (int pos : adjacent)
        {
            if (bord->getCharacter(pos) != nullptr)
            {
                targets.push_back(bord->getCharacter(pos));
            }
        }

        if (targets.empty())
        {
            cout << "No adjacent fighters to attack!\n";
            return;
        }

        cout << "Select target:\n";
        for (int i = 0; i < targets.size(); i++)
        {
            cout << i + 1 << ". " << targets[i]->getName() << endl;
        }

        int k;
        cout << "Choice: ";
        cin >> k;

        if (k >= 1 && k <= targets.size())
        {
            targets[k - 1]->takeDamage(0, 1);
            player->getDeck()->draw();
            cout << "Ability used! 1 damage dealt and 1 card drawn.\n";
        }
        else
        {
            cout << "Invalid choice!\n";
        }
    }
    else
    {
        cout << "Not using ability.\n";
    }
}




Dracula::~Dracula()
{
}
