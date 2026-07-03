#pragma once
#include <string>

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
};
