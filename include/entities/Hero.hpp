#pragma once
#include <string>

class Hero
{
private:
    std::string name;
    int Hp;
    int Ability;
    int move;
    int attack;
    bool isAlive;

public:
    Hero();

    std::string get_name();
    int get_hp();
    int get_ability();
    int get_move();
    int get_attack();
    bool get_isAlive();

    void set_name(std::string name);
    void set_hp(int hp);
    void set_ability(int ability);
    void set_move(int move);
    void set_attack(int attack);
    void set_isAlive(bool isAlive);

    ~Hero();
};