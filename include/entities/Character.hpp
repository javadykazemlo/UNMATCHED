#pragma once
#include <string>
class Bord;
class Player;

class Character
{
private:
    std::string Name;
    int Hp;
    int MaxHp;
    int Move;
    int Attack;// 0 = Melee , 1 = Ranged
    bool IsAlive;
    int Space;
    int Owner;// 1 = first player , 2 = second player

public:
    Character(const std::string& name, int hp, int move, int attack , int owner);

    virtual void ability(Bord* bord , Player* player) = 0;
    bool checkalive() const;
    void takeDamage(int damage ,int attack) ;
    void heal(int amount) ;
    
    void setName(const std::string& name);
    void setHp(int hp);
    void setMove(int move);
    void setAttack(int attack);
    void setIsAlive(bool isAlive);
    void setSpace(int sp);
    void setowner(int ow);
    
    std::string getName() const;
    int getHp() const;
    int getMaxhp() const;
    int getMove() const;
    int getAttack() const;
    bool getIsAlive() const;
    int getSpace() const;
    int getowner() const;


    virtual ~Character();
};



