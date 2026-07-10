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


void Player::chooseCharacter(int choose)
{
    
        std::vector<Card> shCards;
        std::vector<Card> drCards;
    switch (choose)
    {
        case 1://Dracula
            fighters.push_back(new Dracula());
            fighters.push_back(new Sister("Sister Agatha"));
            fighters.push_back(new Sister("Sister Beatrix"));
            fighters.push_back(new Sister("Sister Carmilla"));

            deck = new Deck(shCards, drCards);

            break;
        
        case 2://Sherlock
            fighters.push_back(new Sherlock());
            fighters.push_back(new Dr_watson());

            deck = new Deck(shCards, drCards);

            break;
    }
}


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


Player::~Player()
{
    
}