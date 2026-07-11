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
    Bord bord;

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
    void chooseCharacters();
    void plaseSidekicks(Player& player);
    void playTurn();

    void move(Character* mov);
    
    int boost();
    void resolveCombat(Card& attackCard, Card& defenseCard, Player* attacker, Player* defender);
    void showBurnCards() const;

    void startCombat(Player* attacker, Player* defender);

    bool end_game() const;

    void applyEffect(const Card& card);
    
    std::vector<Card> get_burn_Cards( ) const;
    int getInt();
    int getChoice(std::vector<int> valid);
    bool get_DraculaWon();
    bool get_SherlockWon();

    ~Controller();
};