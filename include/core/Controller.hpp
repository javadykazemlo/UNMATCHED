#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "core/Bord.hpp"
#include "core/Player.hpp"
#include "entities/Character.hpp"
#include "cards/Deck.hpp"
#include "cards/Card.hpp"

class Controller
{
private:
    Player* firstPlayer = nullptr;
    Player* secondPlayer = nullptr;
    Player* current = nullptr;
    Player* enemy = nullptr;
    std::vector<Card> burncards;

    bool DraculaWon;   
    bool sherlockWon;
   // bool Guess;
public:
    Controller();
    
    void choosePlayers(Player player[2]);
    void chooseCharacters(Bord& bord);
    void plaseSidekicks(Bord& bord , Player& player);
    void playTurn(Bord& bord);

    void move(Bord& bord ,Character* mov);
    
    int boost();
    void resolveCombat(Card& attackCard, Card& defenseCard, Player* attacker, Player* defender);
    void showBurnCards() const;

    void startCombat(Player* attacker, Player* defender);
    std::vector<Card> get_burn_Cards( ) const;


    bool end_game() const;
    
    std::vector<Card> get_burn_Cards( ) const;
    int getInt();
    int getChoice(std::vector<int> valid);
    bool get_DraculaWon();
    bool get_SherlockWon();

    ~Controller();
};