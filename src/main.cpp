#include "graphics/GameWindow.hpp"

int main()
{
    GameWindow gw;
    while (gw.isOpen())
    {
        gw.pollAndHandleEvents();
        gw.clear();
        gw.display();
    }
    return 0;
}