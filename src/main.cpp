#include "GameTUI.hpp"
#include <cstdlib>
#include <ctime>

int main() {
    srand(static_cast<unsigned int>(time(0)));
    
    GameTUI game;
    game.run();
    
    return 0;
}