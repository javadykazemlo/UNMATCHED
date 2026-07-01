#pragma once
#include "Character.hpp"
#include <string>

class Dracula: public Character
{
private:

public:
    Dracula();
    void ability() override;

    ~Dracula();
};