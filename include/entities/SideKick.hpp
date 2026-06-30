#pragma once
#include "Character.hpp"
#include <string>

class SideKick : public Character
{
private:

public:
    SideKick();

    std::string get_heroName() const;


    void showSideKicks() const;

    void ability() override;

    ~SideKick();
};