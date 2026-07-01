#include <iostream>
#include <string>
#include "Character.hpp"

using namespace std;

Character::Character(const std::string& name, int hp, int move, int attack)
{
    this->Name = name;
    this->Hp = hp;
    this->Move = move;
    this->Attack = attack;
    this->IsAlive = true;
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



void Character::takeDamage(int defend , int attack) 
{
    if (Hp > 0 &&  attack > defend)
    {
        Hp -= attack ;
        
        if(Hp < 0) 
        {
            Hp = 0;
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




void Character::setName(const std::string& name)
{
    this->Name = name;
}

void Character::setHp(int hp)
{
    Hp = hp;
}

void Character::setMove(int move)
{
    this->Move = move;
}

void Character::setAttack(int attack)
{
    this->Attack = attack;
}

void Character::setIsAlive(bool isAlive)
{
    this->IsAlive = isAlive;
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

int Character::getAttack() const
{
    return Attack;
}

bool Character::getIsAlive() const
{
    return IsAlive;
}

Character::~Character()
{
}