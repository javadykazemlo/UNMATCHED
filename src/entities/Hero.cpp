#include <iostream>
#include <string>
#include "Hero.hpp"

using namespace std;

Hero::Hero()
{
}


std::string Hero::get_name()
{
    return name;
}

int Hero::get_hp()
{
    return Hp;
}

int Hero::get_ability()
{
    return Ability;
}

int Hero::get_move()
{
    return move;
}

int Hero::get_attack()
{
    return attack;
}

bool Hero::get_isAlive()
{
    return isAlive;
}


void Hero::set_name(std::string name)
{
    this->name = name;
}

void Hero::set_hp(int hp)
{
    Hp = hp;
}

void Hero::set_ability(int ability)
{
    Ability = ability;
}

void Hero::set_move(int move)
{
    this->move = move;
}

void Hero::set_attack(int attack)
{
    this->attack = attack;
}

void Hero::set_isAlive(bool isAlive)
{
    this->isAlive = isAlive;
}


Hero::~Hero()
{
}
