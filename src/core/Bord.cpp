#include "Bord.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "entities/Character.hpp"

using namespace std;

Bord::Bord()
{
    spaces[0].adjacent = {1, 2};             //##            
    spaces[1].adjacent = {0, 4};                  
    spaces[2].adjacent = {0, 3, 21};              
    spaces[3].adjacent = {2, 5};                  
    spaces[4].adjacent = {1, 5, 6}; //<<<<<<start              
    spaces[5].adjacent = {3, 4, 20};              
    spaces[6].adjacent = {4, 7, 8};               
    spaces[7].adjacent = {6, 8, 9};               
    spaces[8].adjacent = {6, 7, 9};               
    spaces[9].adjacent = {7, 8, 10, 11, 16};      
    spaces[10].adjacent = {9, 11, 12};            
    spaces[11].adjacent = {9, 10, 13};                
    spaces[12].adjacent = {10};             //##                
    spaces[13].adjacent = {11, 14, 16};           
    spaces[14].adjacent = {13, 15, 31};           
    spaces[15].adjacent = {14, 16}; //<<<<<<start             
    spaces[16].adjacent = {9, 13, 15, 17, 28, 29};
    spaces[17].adjacent = {16, 18};         //##              
    spaces[18].adjacent = {17, 19, 26};           
    spaces[19].adjacent = {18, 22};               
    spaces[20].adjacent = {5, 21};                
    spaces[21].adjacent = {2, 20, 22};            
    spaces[22].adjacent = {19, 21, 23};           
    spaces[23].adjacent = {22, 24};         //##             
    spaces[24].adjacent = {23, 25};                
    spaces[25].adjacent = {24, 26, 27};           
    spaces[26].adjacent = {18, 25, 27, 28};
    spaces[27].adjacent = {25, 26, 28};           
    spaces[28].adjacent = {16, 26, 27, 29};       
    spaces[29].adjacent = {16, 28, 30, 31};       
    spaces[30].adjacent = {29, 31};               
    spaces[31].adjacent = {14, 29, 30};

    spaces[0].zone = {1};
    spaces[1].zone = {1};
    spaces[2].zone = {1};
    spaces[3].zone = {1};
    spaces[4].zone = {1, 2};
    spaces[5].zone = {1, 6};
    spaces[6].zone = {2};
    spaces[7].zone = {2};
    spaces[8].zone = {2};
    spaces[9].zone = {2, 3, 5};
    spaces[10].zone = {3};
    spaces[11].zone = {3};
    spaces[12].zone = {3};
    spaces[13].zone = {4};
    spaces[14].zone = {4};
    spaces[15].zone = {4};
    spaces[16].zone = {4, 5};
    spaces[17].zone = {5};
    spaces[18].zone = {5};
    spaces[19].zone = {5, 6};
    spaces[20].zone = {6};
    spaces[21].zone = {6};
    spaces[22].zone = {6, 7};
    spaces[23].zone = {7};
    spaces[24].zone = {7}; 
    spaces[25].zone = {7};
    spaces[26].zone = {5, 7};
    spaces[27].zone = {7};
    spaces[28].zone = {7};
    spaces[29].zone = {7};
    spaces[30].zone = {7};
    spaces[31].zone = {4, 7};

}


void Bord::addCharacter(int pos , Character* character)
{
    spaces[pos].character = character;
    character->setSpace(pos);
}

void Bord::deletCharacter(int pos)
{
    spaces[pos].character = nullptr;
}

bool Bord::isEmpty(int pos)
{
    return spaces[pos].character == nullptr;
}

bool Bord::checkzone(int zo)
{
    for (int i = 0; i < 32; i++)
    {
        if (find(spaces[i].zone.begin(),spaces[i].zone.end(),zo) != spaces[i].zone.end())
        {
            return true;
        }
    }
    return false;
}


bool Bord::canAttack(int attacktype , int pos)
{
    if(attacktype = 1) //Melee
    {
        for(int neighbor : spaces[pos].adjacent)
        {
            if(spaces[neighbor].character != nullptr)
            return true;
        }

    }
    else //Ranged
    {
        for(int zon : spaces[pos].zone)
        {
            if(spaces[zon].character != nullptr)
            return true;
        }
    }
    return false;
}

vector<Character*> Bord::getAttackCharacters(int attacktype , int pos)
{
    vector<Character*> result;

    if(attacktype = 1)
    {
        for(int neighbor : spaces[pos].adjacent)
        {
            if(spaces[neighbor].character != nullptr)
            result.push_back(spaces[neighbor].character);
        }
    }

    else
    {
        for(int zon : spaces[pos].zone)
        {
            if(spaces[zon].character != nullptr)
            result.push_back(spaces[zon].character);
        }

    }

    return result;
}


vector<Character*> Bord::getZoneCharacters(int pos)
{
    vector<Character*> result;

    

    return result;
}

vector<int> Bord::getEmptyAdjacent(Character* character) const
{
    vector<int> result;

    for (int i = 0; i < 32; i++)
    {
        if (spaces[i].character == character)
        {
            for (int adj : spaces[i].adjacent)
            {
                if (spaces[adj].character == nullptr)
                {
                    result.push_back(adj);
                }
            }

            break;
        }
    }

    return result;
}

vector<int> Bord::getEmptyZone(vector<int> zon) const
{
    vector<int> result;

    for(int j = 0 ; j < zon.size() ; j++)
    {
        for (int i = 0; i < 32; i++)
        {
            if ((find(spaces[i].zone.begin(),spaces[i].zone.end(),zon[j]) != spaces[i].zone.end() ) && spaces[i].character == nullptr)
            {
                result.push_back(i);
            }
        }
    }
    return result;
}



vector<int> Bord::getCharacterAdjacent(Character* character) const
{
    for(int i = 0; i < 32; i++)
    {
        if(spaces[i].character == character)
        return spaces[i].adjacent;
    }
        
    return {};
}
    
    
vector<int> Bord::getCharacterZone(Character* character) const
{
    for(int i = 0; i < 32; i++)
    {
        if(spaces[i].character == character)
        {
            return spaces[i].zone;
        }
    }
    return {};
}

vector<int> Bord::getposAdjacent(int pos) const
{
    return spaces[pos].adjacent;
}

vector<int> Bord::getposZone(int pos) const
{
    return spaces[pos].zone;
}

bool Bord::getSpaceStatus(int pos) const
{
    return spaces[pos].character != nullptr;
}


vector<int> Bord::getSecretPassages(int pos) const
{
    switch (pos)
    {
        case 0:
            return {12, 17, 23};

        case 12:
            return {0, 17, 23};

        case 17:
            return {0, 12, 23};

        case 23:
            return {0, 12 , 17};

        default:
            return {};
    }
}

Character* Bord::getCharacter(int pos) const
{
    return spaces[pos].character;
}
    
Bord::~Bord()
{

}
