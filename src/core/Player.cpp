#include <iostream>
#include <string>
#include "Player.hpp"
#include "entities/Dracula.hpp"
#include "entities/Sherlock.hpp"
#include "entities/Sister.hpp"
#include "entities/Dr_watson.hpp"
#include "entities/invisible_man.hpp"

using namespace std;

Player::Player()
{
    deck = nullptr;
    Age = 0;
    AIcontrolled = false;
    fighterCount = 0;
}


void Player::reset()
{
    for (Character* fighter : fighters)
        delete fighter;
    fighters.clear();

    delete deck;
    deck = nullptr;

    name.clear();
    Age = 0;
    fighterCount = 0;
    AIcontrolled = false;
}

void Player::chooseCharacter(int choose , int owner)
{
    switch (choose)
    {
        case 1://Dracula
        {
            fighters.push_back(new Dracula(owner));
            fighters.push_back(new Sister("Sister Agatha" , owner));
            fighters.push_back(new Sister("Sister Beatrix" , owner));
            fighters.push_back(new Sister("Sister Carmilla" , owner));

            deck = new Deck(1);
            fighterCount = 4;

            break;
        }
        case 2://Sherlock
        {
            fighters.push_back(new Sherlock(owner));
            fighters.push_back(new Dr_watson(owner));

            deck = new Deck(2);
            fighterCount = 2;

            break;
        }
        case 3://invisible man
        {
            fighters.push_back(new invisible_man(owner));

            deck = new Deck(3);
            fighterCount = 1;

            break;
        }
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

void Player::setAI(bool ai)
{
    this->AIcontrolled = ai;
}

bool Player::isAI() const
{
    return AIcontrolled;
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
    if (fighters.empty())
        return nullptr;

    return fighters[0];
}

Character* Player::getsidekick(int i) const
{
    if (i >= 0 && i < (int)fighters.size() && fighters[i] && fighters[i]->checkalive())
        return fighters[i];
    return nullptr;
}

Character* Player::getFighter(int i) const
{
    if (i >= 0 && i < (int)fighters.size())
        return fighters[i];
    return nullptr;
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
    reset();
}