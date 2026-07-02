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
    void dispaly_card();
    bool end_game(Player& p1, Player& p2) const;
    std::vector<Card> get_burn_Cards( ) const;
  
    void plaseSidekicks(int cha , Bord bord);

    ~Controller();
};