#include <iostream>
#include <string>
#include "Player.hpp"
#include "entities/Dracula.hpp"
#include "entities/Sherlock.hpp"
#include "entities/Sister.hpp"
#include "entities/Dr_watson.hpp"


using namespace std;

Player::Player()
{
    deck = nullptr;
    Age = 0;
}


void Player::chooseCharacter(int choose , int owner)
{
    switch (choose)
    {
        case 1://Dracula
            fighters.push_back(new Dracula(owner));
            fighters.push_back(new Sister("Sister Agatha" , owner));
            fighters.push_back(new Sister("Sister Beatrix" , owner));
            fighters.push_back(new Sister("Sister Carmilla" , owner));

            deck = new Deck(1);
            fighterCount = 4;

            break;

        case 2://Sherlock
            fighters.push_back(new Sherlock(owner));
            fighters.push_back(new Dr_watson(owner));

            deck = new Deck(2);
            fighterCount = 2;

            break;
    }
}


void Player::setName(const string& n) 
{
    this->name = n;
}

void Player::setAge(const int& a) 
{
    this->Age = a;
}

void Player::setfighterCount(int count)
{
    this->fighterCount = count;
}


string Player::getName() const
{
    return name;
}
 
int Player::getAge() const
{
    return Age;
}

vector<Character*>& Player::getCharacters()
{
    return fighters;
}

Character* Player::getHero() const
{
    return fighters[0];
}

Character* Player::getsidekick(int i) const
{
    if(fighters[i]->checkalive())
        return fighters[i];
    return {};
}

Deck* Player::getDeck() const 
{ 
    return deck; 
}

int Player::getfighterCount() const
{
    return fighterCount;
}

Player::~Player()
{
    for (Character* fighter : fighters)
    delete fighter;
    

    delete deck;
}