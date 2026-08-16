#include <iostream>
#include <string>
#include "Sherlock.hpp"
#include "core/GuiEffectLogger.hpp"

using namespace std;

Sherlock::Sherlock(int owner): Character("sherlock", 16, 2, 0 , owner , true)
{
}

void Sherlock::ability(Bord& bord , Player* player)
{
    (void)bord;
    (void)player;

    GuiEffectLogger effectLog([](const std::string& message)
    {
        std::clog << message << '\n';
    });

    effectLog << "\nSherlock's ability: cards belonging to Holmes or Watson "
                  "can never be disabled by an opponent's card effect.\n";
}