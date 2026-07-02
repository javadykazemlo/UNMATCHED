#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "core/Bord.hpp"
#include "entities/Character.hpp"
#include "cards/Deck.hpp"


class Player 
{
private:
    std::string name;
    int Age;

    std::vector<Character*> fighters;
public:
    Player();
    void chooseCharacter(int );

    void setName(const std::string& n);
    void setAge(const int& a);

    std::string getName() const;
    int getAge() const;
    Character* getHero() const;
    Character* getsidekick(int i) const;


    ~Player();

};
