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
    //this->IsAlive = true;
    this->MaxHp = hp;
    this->Owner = owner;
    this->ishero = her;
}


bool Character::checkalive() const
{
    return Hp > 0;
}



void Character::takeDamage(int damage) {
    if (Hp > 0) {
        if (damage > 0) {
            Hp -= damage;
            if (Hp <= 0) {
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


