#include <iostream>
#include <string>
#include <vector>
#include "Dracula.hpp"

using namespace std;

Dracula::Dracula(): Character("Dracula", 13, 2, 0)
{
    currentPlayer = nullptr;
    gameBoard = nullptr;
}



void Dracula::setCurrentPlayer(Player* player)
{
    currentPlayer = player;
}

void Dracula::setGameBoard(Bord* board)
{
    gameBoard = board;
}

void Dracula::ability()
{

    cout << "Do you want to use Dracula's ability? (y/n): ";
    char choice;
    cin >> choice;  

    if (choice == 'y' || choice == 'Y')
    {
        vector<int> adjacent = gameBoard->getAdjacent(this);
        vector<Character*> targets;

        for (int pos : adjacent)
        {
            if (gameBoard->getCharacterName(pos) != nullptr)
            {
                targets.push_back(gameBoard->getCharacterName(pos));
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
            currentPlayer->getDeck()->cardHandSH();
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

void Dracula::setCurrentPlayer(Player* player)
{
    currentPlayer = player;
}

void Dracula::setGameBoard(Bord* board)
{
    gameBoard = board;
}



Dracula::~Dracula()
{
}
