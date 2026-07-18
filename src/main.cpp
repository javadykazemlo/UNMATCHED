#include <iostream>
#include <cstdlib>
#include <ctime>
#include <memory>
#include "Controller.hpp"
#include "Player.hpp"
#include "Bord.hpp"
#include "GameTUI.hpp"

int main() 
{
    srand(static_cast<unsigned int>(time(0)));
    
    Controller controller;
    Player player[2];
    
    // انتخاب بازیکنان
    controller.choosePlayers(player);
    
    // انتخاب شخصیت‌ها
    controller.chooseCharacters();
    
    // اجرای TUI
    GameTUI tui(&controller);
    tui.run();
    
    std::cout << "\n Thanks for playing Unmatched!\n";
    return 0;
}