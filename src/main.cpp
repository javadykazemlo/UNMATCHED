#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

#include "Controller.hpp"
#include "Player.hpp"
#include "Bord.hpp"
#include "GameTUI.hpp"
int main()
{
    srand(static_cast<unsigned int>(time(0))) ;

    Controller controller;
    Player player[2];


    controller.choosePlayers(player);

    controller.chooseCharacters();
    

    GameTUI tui(&controller);
    tui.run();
  	return 0;
}
  