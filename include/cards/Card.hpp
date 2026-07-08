#pragma once
#include <string>
#include "core/Bord.hpp"
#include "core/Player.hpp"
#include "entities/Character.hpp"
#include "cards/Deck.hpp"
struct Card 
{

    std::string name;
    std::string owner;
    std::string type;
    int attack;
    int defense;
    std::string timing;
    int boost;
    int quantity;
    bool isExtra;
    std::string effect;
 

     void applyEffect(Player* pl,Player* enemyPl , Bord*bo);
};
