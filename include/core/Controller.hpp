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
    Player* firstPlayer = nullptr;
    Player* secondPlayer = nullptr;
    Player* current = nullptr;
    Player* enemy = nullptr;
    std::vector<Card> burncards;
public:
    Controller();
    
    void choosePlayers(Player player[2]);
    void chooseCharacters(Bord& bord);
    void plaseSidekicks(Bord& bord , Player& player);
    void playTurn(Bord& bord);

    void move(Bord& bord , int mov);
    
    void resolveCombat(Card& attackCard, Card& defenseCard, Player& attacker, Player& defender);
    void showBurnCards() const;
    void startCombat(Player& attacker, Player& defender);
    std::vector<Card> get_burn_Cards( ) const;

    bool end_game() const;
    

    int getInt();
    int getChoice(std::vector<int> valid);

    ~Controller();
};