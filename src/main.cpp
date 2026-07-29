#include "GameTUI.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "Controller.hpp"
#include "Player.hpp"
#include "Bord.hpp"
#include "GameTUI.hpp"

using namespace std;

int main()
{
    srand(static_cast<unsigned int>(time(0))) ;

    Controller controller;
    Player player[2];

    cout << "1. New Game\n2. Load Game\nChoose an option: ";
    int choice = 0;
    cin >> choice;

    if (choice == 2)
    {
        if (controller.LoadGame(player))
        {
            cout << "\nResuming saved game...\n";
        }
        else
        {
            cout << "\nNo valid save found, starting a new game instead.\n";
            GameTUI::showWelcome();
            controller.choosePlayers(player);
        }
    }
    else
    {
        GameTUI::showWelcome();
        controller.choosePlayers(player);
    }

    controller.playTurn();

    controller.SaveGame();

  	return 0;
}