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
    bool AIcontrolled;
public:
    Player();
    void chooseCharacter(int , int owner);
    void reset();

    void setName(const std::string& n);
    void setAge(const int& a);
    void setfighterCount(int count);
    void setAI(bool ai);
    bool isAI() const;

    std::string getName() const;
    int getAge() const;
    std::vector<Character*>& getCharacters();
    Character* getHero() const;
    Character* getsidekick(int i) const;
    Character* getFighter(int i) const;
    int getfighterCount() const;

    Deck* getDeck() const ;
    ~Player();

};