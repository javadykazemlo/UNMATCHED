#pragma once
#include <string>

class Charactore
{
private:
    std::string name;
    int Hp;
    int MaxHp;
    int move;
    int attack;
    bool isAlive;

public:
    Charactore(const std::string& name, int hp, int move, int attack);

    std::string get_name() const;
    int get_hp() const;
    int get_Maxhp() const;
    int get_move() const;
    int get_attack() const;
    bool get_isAlive() const;
    bool checkalive() const;

    virtual void ability() = 0;
    void takeDamage(int damage ,int attack) ;
    void heal(int amount) ;

    void set_name(const std::string& name);
    void set_hp(int hp);
    void set_move(int move);
    void set_attack(int attack);
    void set_isAlive(bool isAlive);

    virtual ~Charactore();
};