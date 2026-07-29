#pragma once
#include <vector>
#include "entities/Character.hpp"
#include "core/Bord.hpp"
#include "core/Player.hpp"

class invisible_man : public Character
{
private:
    std::vector<int> mistTokens; 
public:
    invisible_man(int owner);

    void ability(Bord& bord , Player* player) override;

    void setMistToken(int index , int pos);
    int getMistToken(int index) const;
    std::vector<int> getMistTokens() const;

    bool isOnMist(int pos) const;
    bool isMistPosition(int pos) const;

    ~invisible_man() = default;
};