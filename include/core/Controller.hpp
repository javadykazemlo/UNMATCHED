#pragma once
#include <vector>
#include "Card.hpp"
#include "Character.hpp"

class Controller
{
private:
    std::vector<Card> burncards;

public:
    Controller();

    void calculation_cart(vector<Card>& deck,int index,int attack,int defend);
    std::vector<Card> get_burn_Cards( ) const;

    ~Controller();
};


