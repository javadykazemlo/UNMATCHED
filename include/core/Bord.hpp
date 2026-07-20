#pragma once
#include <iostream>
#include <vector>
#include <string>
class Character;

struct cell
{
    std::vector<int> adjacent;
    std::vector<int> zone;
    Character* character = nullptr;
};

class Bord
{
private:
    cell spaces[32];
public:
    Bord();
    
    void addCharacter(int, Character* character);
    void deletCharacter(int);
    bool isEmpty(int pos);
    bool canAttack(int attacktype , int pos);
    
    bool checkzone(int z);

    std::vector<Character*> getAttackCharacters(int attacktype , int pos);
    std::vector<Character*> getZoneCharacters(int pos);


    std::vector<int> getEmptyAdjacent(Character* character) const;
    std::vector<int> getEmptyZone(std::vector<int> zon) const;

    std::vector<int> getCharacterAdjacent(Character* character) const;
    std::vector<int> getCharacterZone(Character* character) const;
    std::vector<int> getposAdjacent(int pos) const;
    std::vector<int> getposZone(int pos) const;

    bool getSpaceStatus(int pos) const;

    std::vector<int> getSecretPassages(int pos) const;
    Character* getCharacter(int pos) const;

    ~Bord();

};