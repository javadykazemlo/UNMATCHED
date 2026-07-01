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

    void Resolve_Combat(vector<Card>& deck,int index,int attack,int defend);
    void showBurnCards() const;
    void selectCard_SH()const;
    void selectCard_DR()const;
    bool end_game() const;
    std::vector<Card> get_burn_Cards( ) const;

    ~Controller();
};


