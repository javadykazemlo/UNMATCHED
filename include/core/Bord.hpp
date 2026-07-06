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
    

    bool checkzone(int z);
    std::vector<int> getCellThisZone(std::vector<int> zon);

    void addCharacter(int, Character* character);
    void deletCharacter(int);
    bool isEmpty(int pos);


    std::vector<int> getAdjacent(Character* character) const;
    std::vector<int> getZone(Character* character) const;
    bool getSpaceStatus(int pos) const;
    Character* getCharacterName(int pos) const;

    ~Bord();
};



