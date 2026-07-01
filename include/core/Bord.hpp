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
    std::vector<int> getCellThisZone(std::vector<int> zon);

    void addCharacter(int, std::string name);
    void deletCharacter(int);

    std::vector<int> getAdjacent(std::string) const;
    std::vector<int> getZone(std::string) const;
    bool getSpaceStatus(int cell) const;
    std::string getCharacterName(int cell) const;

    ~Bord();
};



