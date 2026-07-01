#pragma once
#include <string>

class Character
{
private:
    std::string Name;
    int Hp;
    int MaxHp;
    int Move;
    int Attack;
    bool IsAlive;

public:
    Character(const std::string& name, int hp, int move, int attack);

    
    virtual void ability() = 0;
    bool checkalive() const;
    void takeDamage(int damage ,int attack) ;
    void heal(int amount) ;
    
    void setName(const std::string& name);
    void setHp(int hp);
    void setMove(int move);
    void setAttack(int attack);
    void setIsAlive(bool isAlive);
    
    std::string getName() const;
    int getHp() const;
    int getMaxhp() const;
    int getMove() const;
    int getAttack() const;
    bool getIsAlive() const;

    virtual ~Character();
};