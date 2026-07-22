#include <iostream>
#include <string>
#include "Character.hpp"

using namespace std;

Character::Character(const std::string& name, int hp, int move, int attacktyp , int owner , bool her)
{
    this->Name = name;
    this->Hp = hp;
    this->Move = move;
    this->Attacktype = attacktyp;
    //this->IsAlive = true;
    this->MaxHp = hp;
    this->Owner = owner;
    this->ishero = her;
}


bool Character::checkalive() const
{
    return Hp > 0;
}



void Character::takeDamage(int damage) {
    if (Hp > 0) {
        if (damage > 0) {
            Hp -= damage;
            if (Hp <= 0) {
                // FIX: قبلاً اینجا Space = -1 می‌شد، یعنی همون لحظه‌ی مرگ
                // موقعیت کاراکتر روی نقشه گم می‌شد. بعدش هرجا کد می‌خواست
                // با getSpace() کاراکتر رو از روی تخته پاک کنه
                // (bord.deletCharacter(character->getSpace())) در واقع
                // deletCharacter(-1) صدا می‌شد که دسترسی خارج از رنج
                // آرایه‌ی spaces[32] بود و باعث کرش می‌شد.
                // حالا Space دست‌نخورده می‌مونه تا هرکسی که مرگ رو تشخیص
                // می‌ده بتونه با getSpace() موقعیت درست رو بگیره، از تخته
                // پاکش کنه، و *بعدش* خودش صراحتاً setSpace(-1) بزنه.
                Hp = 0;
            }
        }
    }
}


void Character::heal(int boost ) 
{
    Hp += boost;
    if(Hp > MaxHp) 
    {
        Hp = MaxHp;
    }
}


void Character::setHp(int hp)
{
    Hp = hp;
}

void Character::setSpace(int sp)
{
    this->Space = sp;
}

void Character::setowner(int ow)
{
    this->Owner = ow;
}


std::string Character::getName() const
{
    return Name;
}

int Character::getHp() const
{
    return Hp;
}

int Character::getMaxhp() const
{
    return MaxHp;
}


int Character::getMove() const
{
    return Move;
}

int Character::getAttacktype() const
{
    return Attacktype;
}

int Character::getSpace() const
{
    return Space;
}

int Character::getowner() const
{
    return Owner;
}

int Character::isHero() const
{
    return ishero;
}