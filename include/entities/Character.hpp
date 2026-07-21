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
    int Attacktype;// 0 = Melee , 1 = Ranged
    int Space;
    int Owner;// 1 = first player , 2 = second player
    bool ishero;
public:
    Character(const std::string& name, int hp, int move
        , int attacktyp , int owner , bool her);

    virtual void ability(Bord bord , Player* player) = 0;
    bool checkalive() const;
    void takeDamage(int damage);
    void heal(int amount) ;
    
    void setHp(int hp);
    void setSpace(int sp);
    void setowner(int ow);
    
    std::string getName() const;
    int getHp() const;
    int getMaxhp() const;
    int getMove() const;
    int getAttacktype() const;
    int getSpace() const;
    int getowner() const;
    int isHero() const;

    virtual ~Character() = default;
};



