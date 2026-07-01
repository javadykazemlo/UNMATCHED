#pragma once
#include <string>
#include "Character.hpp"

class Dr_watson : public Character
{
private:
    // std::string name;
public:
    Dr_watson();

    void ability() override;

    ~Dr_watson();
};