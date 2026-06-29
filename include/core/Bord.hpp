#pragma once
#include <iostream>
#include <vector>
#include <string>

struct cell
{
    std::vector<int> adjacent;
    std::vector<int> zone;
    bool Character = false;
    std::string CharacterName = "";
};

class Bord
{
private:
    cell spaces[32];
public:
    Bord();
    

    bool checkzone(int z);
    std::vector<int> getCellThisZone(int zo);

    void addCharacter(int, string name);
    void deletCharacter(int);

    std::vector<int> getAdjacent(int cell) const;
    std::vector<int> getZone(int cell) const;
    bool getSpaceStatus(int cell) const;
    std::string getCharacterName(int cell) const;

    ~Bord();
};



