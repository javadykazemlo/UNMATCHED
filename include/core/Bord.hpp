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
    
    bool checkzone(int z);
    std::vector<int> getEmptyAdjacent(Character* character) const;
    std::vector<int> getEmptyZone(std::vector<int> zon) const;

    std::vector<int> getAdjacentCharacter(Character* character) const;
    std::vector<int> getZoneCharacter(Character* character) const;
    std::vector<int> getAdjacentpos(int pos) const;
    std::vector<int> getZonepos(int pos) const;
    bool getSpaceStatus(int pos) const;

    std::vector<int> getSecretPassages(int pos) const;
    Character* getCharacter(int pos) const;

    ~Bord();

    //void check();

};

