#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "core/Bord.hpp"
#include "core/Player.hpp"
#include "entities/Character.hpp"
#include "cards/Deck.hpp"

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
  
    void plaseSidekicks(int cha , Bord bord);

    ~Controller();
};