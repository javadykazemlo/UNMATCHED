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

    bool DraculaWon;   
    bool sherlockWon;
   // bool Guess;
public:
    Controller();
    
    void choosePlayers(Player player[2]);
    void chooseCharacters();
    void plaseSidekicks(Player& player);
    void playTurn();

    void move();
    int boost();
    
    void Scheme();

    void startCombat();
    Card chooseCombatCard(Player* player , Character* fighter, bool attack);
    void resolveCombat(Card& attackCard, Card& defenseCard);

    bool end_game() const;

    void applyEffect(const Card& card);
    
    int getInt();
    int getChoice(std::vector<int> valid);
    bool get_DraculaWon();
    bool get_SherlockWon();

    ~Controller();
};