#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

#include "Controller.hpp"
#include "Player.hpp"
#include "Bord.hpp"

int main()
{
    srand(static_cast<unsigned int>(time(0))) ;

    Controller controller;
    Bord bord;
    Player player[2];


    controller.choosePlayers(player);

    controller.chooseCharacters(bord);
    
    while(!controller.end_game())
    {
        controller.playTurn(bord);
    }

    
  	return 0;
}
  