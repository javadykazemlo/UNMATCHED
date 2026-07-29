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
    bool cancelEffectIM = false;
    
    int gamerand = 0;
    bool GuessElementary = false;

    Player* activeDecider = nullptr;

    enum class AIDecision
    {
        Generic,        
        ActionChoice,   
        FighterSelect,  
        MoveDestination,
        AttackTarget,   
        CardSelect,     
        BoostChoice
    };
    enum class AICardPurpose { Attack, Defense, Boost, Scheme };

    AIDecision aiDecisionKind = AIDecision::Generic;
    AICardPurpose aiCardPurpose = AICardPurpose::Attack;
    std::vector<Character*> aiCharacterOptions;
    Character* aiMovingCharacter = nullptr;
    Character* aiCardFighter = nullptr; 

    int boardDistance(int from, int to);
    void damageAllFighters(Player* p, int damage);

    int aiScoreAction(int action, Player* decider);
    int aiScoreFighter(int idx, Player* decider);
    int aiScoreMove(int destination, Player* decider);
    int aiScoreAttackTarget(int idx, Player* decider);
    int aiScoreCardChoice(int idx, Player* decider);

    int aiChoose(const std::vector<int>& valid, Player* decider);
    bool aiYesNo(Player* decider);
    int aiInt(Player* decider);
    void aiThink(Player* decider);

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
    bool getYesNo();
    
    Bord& getBord();
    Player* getCurrentPlayer();
    Player* getEnemyPlayer();
    Character* getCharacterAt(int position);
    bool isGameOver();
    
    void SaveGame(const std::string& filename = "save.json");
    bool LoadGame(Player player[2], const std::string& filename = "save.json");
    bool end_game() const;

    void applyEffect(Card& card, Card& enemycard, Player* self, Player* opponent, Character* attacker, Character* defender, bool woncombat);
    void applyEffectScheme(Card& card, Player* self, Player* opponent, Character* attacker);

    ~Controller() = default;
};