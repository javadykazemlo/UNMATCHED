#include <cstdlib>
#include <ctime>
#include "Controller.hpp"

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    Controller controller;
    Player player[2];

    controller.startMenu(player);
    controller.playTurn();

  	return 0;
}