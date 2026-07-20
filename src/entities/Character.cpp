#include <iostream>
#include <string>
#include "Character.hpp"

using namespace std;

Character::Character(const std::string& name, int hp, int move, int attacktyp , int owner , bool her)
{
    this->Name = name;
    this->Hp = hp;
    this->Move = move;
    this->Attacktype = attacktyp;
    this->IsAlive = true;
    this->MaxHp = hp;
    this->Owner = owner;
    this->ishero = her;
}


bool Character::checkalive() const
{
    if (Hp == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}



void Character::takeDamage(int defend, int attack) {
    if (Hp > 0) {
        int damage = attack - defend;
        if (damage > 0) {
            Hp -= damage;
            if (Hp <= 0) {
                IsAlive = false;
                Space = -1;
                Hp = 0;
            }
        }
    }
}


void Character::heal(int boost ) 
{
    Hp += boost;
    if(Hp > MaxHp) 
    {
        Hp = MaxHp;
    }
}


void Character::setHp(int hp)
{
    Hp = hp;
}

void Character::setIsAlive(bool isAlive)
{
    this->IsAlive = isAlive;
}

void Character::setSpace(int sp)
{
    this->Space = sp;
}

void Character::setowner(int ow)
{
    this->Owner = ow;
}


std::string Character::getName() const
{
    return Name;
}

int Character::getHp() const
{
    return Hp;
}

int Character::getMaxhp() const
{
    return MaxHp;
}


int Character::getMove() const
{
    return Move;
}

int Character::getAttacktype() const
{
    return Attacktype;
}

bool Character::getIsAlive() const
{
    return IsAlive;
}

int Character::getSpace() const
{
    return Space;
}

int Character::getowner() const
{
    return Owner;
}

int Character::isHero() const
{
    return ishero;
}


