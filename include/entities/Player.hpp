#pragma ones
#include <iostream>
#include <string>
#include <vector>
class Character;
class Deck;
class Bord;

class Player 
{
private:
    std::string name;
    int Age;
    std::string CharacterName;
    Character* hero;
    vector<Character*> sideKick;
    Deck* deck;
    Bord bord;
public:
    Player();
    void chooseCharacter(int , int);

    void setName(const std::string& n);
    void setAge(const int& a);
    void setCharacterName(const std::string& cn);

    std::string getName() const;
    int getAge() const; 
    std::string getCharacterName() const;

};
