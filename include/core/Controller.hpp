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

    Player* current = nullptr;
    Player* enemy = nullptr;

    bool cancelEffectDR = false;
    bool cancelEffectSH = false;
    
    int gamerand = 0;
    bool GuessElementary = false;
    
public:
    Controller() = default;
    
    void choosePlayers(Player player[2]);
    void chooseCharacters();
    void plaseSidekicks(Player& player);

    void playTurn();

    void move(int mov, Character* selected);
    int boost();
    void Scheme();
    void startCombat();
    Card chooseCombatCard(Player* player, Character* fighter, bool attack);
    void resolveCombat(Card& attackCard, Card& defenseCard, Character* attacker, Character* defender);

    int getInt();
    int getChoice(std::vector<int> valid);
    
    Bord& getBord();
    Player* getCurrentPlayer();
    Player* getEnemyPlayer();
    Character* getCharacterAt(int position);
    bool isGameOver();
    
    bool end_game() const;

    void applyEffect(Card& card, Card& enemycard, Player* self, Player* opponent, Character* attacker, Character* defender, bool woncombat);
    void applyEffectScheme(Card& card, Player* self, Player* opponent, Character* attacker);

    ~Controller() = default;
};