
#include <iostream>
#include <string>
#include "Player.hpp"

using namespace std;

Player::Player()
{
}


void Player::chooseCharacter(int choose , int pos)
{
    switch (choose)
    {
        break;
        case 1://Dracula
        
        
        
        case 2://Sherlock
        
        break;
        
    }
    
    
    bord.addCharacter( pos , CharacterName );
}


void Player::setName(const string& n) 
{
    name = n;
}

void Player::setAge(const int& a) 
{
    Age = a;
}

void Player::setCharacterName(const std::string& cn)
{
    CharacterName = cn;
}


string Player::getName() const
{
    return name;
}
 
int Player::getAge() const
{
    return Age;
}

std::string Player::getCharacterName() const
{
    return CharacterName;
}


Player::~Player()
{
}