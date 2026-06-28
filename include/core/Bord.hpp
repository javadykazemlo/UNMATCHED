#pragma once
#include <iostream>
#include <vector>

struct cell
{
    std::vector<int> adjacent;
    std::vector<int> zone;
};

class Bord
{
private:
    cell spaces[32];
public:
    Bord();
    

    

    ~Bord();
};