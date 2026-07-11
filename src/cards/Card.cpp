#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "cards/Card.hpp"


using namespace std;


Card::Card(string nam , owner own , CardType typ , int att , timing tim , int bos , string efec)
{
    this-> Name = nam;
    this-> Owner = own;
    this-> Type = typ;
    this-> Attack = att; 
    this-> Timing = tim; 
    this-> Boost = bos; 
    this->effect = efec;
}

bool Card::isHero() const
{
    return Owner == owner::Hero;
}

bool Card::issideKick() const
{
    return Owner == owner::SideKick;
}

bool Card::isAnyowner() const
{
    return Owner == owner::Any;
}


bool Card::isAttack() const
{
    return Type == CardType::Attack;
}

bool Card::isDefense() const
{
    return Type == CardType::Defense;
}

bool Card::isVersatile() const
{
    return Type == CardType::Versatile;
}

bool Card::isScheme() const
{
    return Type == CardType::Scheme;
}


bool Card::isNonetiming() const
{
    return Timing == timing::None;
}

bool Card::isBeforeCombat() const
{
    return Timing == timing::BeforeCombat;
}

bool Card::isDuringCombat() const
{
    return Timing == timing::DuringCombat;
}

bool Card::isAfterCombat() const
{
    return Timing == timing::AfterCombat;
}

std::string Card::getOwnerString() const
{
    switch (Owner)
    {
        case owner::Hero:
            return "Hero";

        case owner::SideKick:
            return "SideKick";

        case owner::Any:
            return "Any";
    }

    return "Unknown";
}

string Card::getTypeString() const
{
    switch (Type)
    {
        case CardType::Attack:
            return "Attack";

        case CardType::Defense:
            return "Defense";

        case CardType::Versatile:
            return "Versatile";

        case CardType::Scheme:
            return "Scheme";
    }

    return "Unknown";
}

std::string Card::getTimingString() const
{
    switch (Timing)
    {
        case timing::None:
            return "None";

        case timing::BeforeCombat:
            return "Before Combat";

        case timing::DuringCombat:
            return "During Combat";

        case timing::AfterCombat:
            return "After Combat";
    }

    return "Unknown";
}

string Card::getName() const
{
    return Name;
}

owner Card::getOwner() const 
{
    return Owner;
}

CardType Card::getType() const
{
    return Type;
}

int Card::getAttack() const 
{
    return Attack;
}

timing Card::getTiming() const
{
    return Timing;
}

int Card::getBoost() const 
{
    return Boost;
}

string Card::geteffect() const
{
    return effect;
}


Card::~Card()
{
}





