
#include <iostream>
#include <string>
#include "Player.hpp"

using namespace std;

void Player::setName(const string& n) 
{
    name = n;
}

void Player::setAge(const int& a) 
{
    Age = a;
}


string Player::getName() const
{
    return name;
}
 
int Player::getAge() const
{
    return Age;
}
