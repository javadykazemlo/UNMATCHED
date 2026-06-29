#pragma once
#include "Charactore.hpp"
#include <string>

class SideKick : public Charactore
{
private:

public:
    SideKick();

    std::string get_heroName() const;


    void showSideKicks() const;

    void ability() override;

    ~SideKick();
};