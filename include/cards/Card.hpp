#pragma once
#include <string>

#include "entities/Character.hpp"

enum class owner
{
    Hero,
    SideKick,
    Any
};

enum class CardType
{
    Attack,
    Defense,
    Versatile,
    Scheme
};

enum class timing
{
    None,
    BeforeCombat,
    DuringCombat,
    AfterCombat
};

class Card
{
private:
    std::string Name;
    owner Owner; 
    CardType Type; 
    int Attack;
    timing Timing;
    int Boost;
    std::string effect;
public:
    Card() = default;
    Card(std::string nam , owner own , CardType typ , int att , timing tim , int bos , std::string efec);

    bool isHero() const;
    bool issideKick() const;
    bool isAnyowner() const;

    bool isAttack() const;
    bool isDefense() const;
    bool isScheme() const;
    bool isVersatile() const;

    bool isNonetiming() const;
    bool isBeforeCombat() const;
    bool isDuringCombat() const;
    bool isAfterCombat() const;

    std::string getOwnerString() const;
    std::string getTypeString() const;
    std::string getTimingString() const;

    std::string getName() const;
    owner getOwner() const;
    CardType getType() const;
    int getAttack() const;
    timing getTiming() const;
    int getBoost() const;
    std::string geteffect() const;

    ~Card();
};