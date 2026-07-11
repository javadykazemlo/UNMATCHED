#pragma once
#include "entities/Character.hpp"
#include "core/Bord.hpp"
#include "core/Player.hpp"

class Sister : public Character
{
private:
    
public:
    Sister(std::string name , int owner);

    void ability(Bord* bord , Player* player) override;

    ~Sister();
};