#pragma once
#include "Character.hpp"
#include <string>

class Hero: public Character
{
private:

public:
    Hero();
    void showHeroes() const;
   

    ~Hero();
};