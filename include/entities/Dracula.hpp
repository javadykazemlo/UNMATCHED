#pragma once
#include "entities/Character.hpp"
#include "core/Bord.hpp"
#include "core/Player.hpp"

class Dracula : public Character
{
private:

public:
    Dracula(int owner);

    void ability(Bord bord , Player* player) override;

    ~Dracula();
};