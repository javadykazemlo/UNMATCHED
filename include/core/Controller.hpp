#pragma ones
#include <iostream>
#include <string>
#include <vector>
#include "core/Bord.hpp"
#include "core/Player.hpp"
#include "entities/Character.hpp"
#include "cards/Deck.hpp"


class Controller 
{
private:
    Bord bord;
public:
    Controller();

    void plaseSidekicks(int cha);
   
    ~Controller();
};