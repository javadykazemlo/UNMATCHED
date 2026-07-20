#include "GameTUI.hpp"
#include <cstdlib>
#include <ctime>

#include "Controller.hpp"
#include "Player.hpp"
#include "Bord.hpp"
#include "GameTUI.hpp"

int main()
{
    srand(static_cast<unsigned int>(time(0))) ;

    Controller controller;
    Player player[2];

    GameTUI::showWelcome();

    controller.choosePlayers(player);

    controller.chooseCharacters();
    
    controller.playTurn();
  
  	return 0;
}