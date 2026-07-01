#pragma once
#include "Character.hpp"
#include <string>

class Sherlock: public Character
{
private:

public:
    Sherlock();
    void ability() override;

    ~Sherlock();
};