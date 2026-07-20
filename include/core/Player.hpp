#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "core/Bord.hpp"
#include "entities/Character.hpp"
#include "cards/Deck.hpp"
class Card;


class Player 
{
private:
    std::string name;
    int Age;
    int fighterCount;

    std::vector<Character*> fighters;
    Deck* deck;
public:
    Player();
    void chooseCharacter(int , int owner);

    void setName(const std::string& n);
    void setAge(const int& a);
    void setfighterCount(int count);

    std::string getName() const;
    int getAge() const;
    std::vector<Character*>& getCharacters();
    Character* getHero() const;
    Character* getsidekick(int i) const;
    int getfighterCount() const;

    Deck* getDeck() const ;
    ~Player();

};