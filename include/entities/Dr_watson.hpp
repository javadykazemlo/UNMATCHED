#pragma once
#include "entities/Character.hpp"
#include "core/Bord.hpp"
#include "core/Player.hpp"

class Dr_watson : public Character
{
private:
    
public:
    Dr_watson(int owner);

    void ability(Bord bord , Player* player) override;

    ~Dr_watson();
};