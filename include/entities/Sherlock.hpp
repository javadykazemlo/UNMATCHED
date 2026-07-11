#pragma once
#include "entities/Character.hpp"
#include "core/Bord.hpp"
#include "core/Player.hpp"

class Sherlock: public Character
{
private:

public:
    Sherlock(int owner);
    
    void ability(Bord* bord , Player* player) override;

    ~Sherlock();
};