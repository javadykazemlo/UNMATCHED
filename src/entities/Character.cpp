#include <iostream>
#include <string>
#include "Character.hpp"

using namespace std;

Character::Character(const std::string& name, int hp, int move, int attack)
{
    this->name = name;
    this->Hp = hp;
    this->move = move;
    this->attack = attack;
    this->isAlive = true;
}

std::string Character::get_name() const
{
    return name;
}

int Character::get_hp() const
{
    return Hp;
}

int Character::get_Maxhp() const
{
    return MaxHp;
}


int Character::get_move() const
{
    return move;
}

int Character::get_attack() const
{
    return attack;
}

bool Character::get_isAlive() const
{
    return isAlive;
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




void Character::set_name(const std::string& name)
{
    this->name = name;
}

void Character::set_hp(int hp)
{
    Hp = hp;
}

void Character::set_move(int move)
{
    this->move = move;
}

void Character::set_attack(int attack)
{
    this->attack = attack;
}

void Character::set_isAlive(bool isAlive)
{
    this->isAlive = isAlive;
}

Character::~Character()
{
}