#include <iostream>
#include <string>
#include "Charactore.hpp"

using namespace std;

Charactore::Charactore(const std::string& name, int hp, int move, int attack)
{
    this->name = name;
    this->Hp = hp;
    this->move = move;
    this->attack = attack;
    this->isAlive = true;
}

std::string Charactore::get_name() const
{
    return name;
}

int Charactore::get_hp() const
{
    return Hp;
}

int Charactore::get_move() const
{
    return move;
}

int Charactore::get_attack() const
{
    return attack;
}

bool Charactore::get_isAlive() const
{
    return isAlive;
}

bool Charactore::checkalive() const
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

void Charactore::set_name(const std::string& name)
{
    this->name = name;
}

void Charactore::set_hp(int hp)
{
    Hp = hp;
}

void Charactore::set_move(int move)
{
    this->move = move;
}

void Charactore::set_attack(int attack)
{
    this->attack = attack;
}

void Charactore::set_isAlive(bool isAlive)
{
    this->isAlive = isAlive;
}

Charactore::~Charactore()
{
}