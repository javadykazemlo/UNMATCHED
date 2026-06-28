#pragma ones
#include <iostream>
#include <string>

class Player 
{
private:
    std::string name;
    int Age;

public:
    void setName(const std::string& n);
    void setAge(const int& a);

    std::string getName() const;
    int getAge() const; 
};