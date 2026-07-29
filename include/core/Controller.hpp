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

    // Which Player is currently the one making a decision. getInt()/getChoice()/getYesNo()
    // check this to decide whether to read the terminal or ask the AI decision stub.
    // Falls back to `current` when left null. Set it right before any block of decisions
    // that belongs to a specific player (see plaseSidekicks, chooseCombatCard, resolveCombat,
    // applyEffect/applyEffectScheme, and the few spots inside them that address the opponent).
    Player* activeDecider = nullptr;

    // ── AI heuristic context ───────────────────────────────────────────────
    // getChoice()/getYesNo() only ever see a bare list of ints, so to score them
    // meaningfully we tag *what kind* of decision it is right before the call,
    // plus whatever extra context that kind of scoring needs. Always reset to
    // Generic right after, so it never leaks into the next unrelated call.
    enum class AIDecision
    {
        Generic,        // no scoring context available -> fall back to a light random pick
        ActionChoice,   // 1..4 : Maneuver / Scheme / Attack / End turn
        FighterSelect,  // valid[i] indexes into aiCharacterOptions
        MoveDestination,// valid[i] is a board position; aiMovingCharacter is who's moving
        AttackTarget,   // valid[i] indexes into aiCharacterOptions (enemy fighters in range)
        CardSelect,     // valid[i] indexes into decider's hand; aiCardPurpose says why
        BoostChoice     // the "use boost?" y/n
    };
    enum class AICardPurpose { Attack, Defense, Boost, Scheme };

    AIDecision aiDecisionKind = AIDecision::Generic;
    AICardPurpose aiCardPurpose = AICardPurpose::Attack;
    std::vector<Character*> aiCharacterOptions;
    Character* aiMovingCharacter = nullptr;
    Character* aiCardFighter = nullptr; // who the CardSelect decision is being made for (ownership check)

    int boardDistance(int from, int to);

    int aiScoreAction(int action, Player* decider);
    int aiScoreFighter(int idx, Player* decider);
    int aiScoreMove(int destination, Player* decider);
    int aiScoreAttackTarget(int idx, Player* decider);
    int aiScoreCardChoice(int idx, Player* decider);

    int aiChoose(const std::vector<int>& valid, Player* decider);
    bool aiYesNo(Player* decider);
    int aiInt(Player* decider);
    void aiThink(Player* decider); // small pause + "thinking..." message before the AI answers

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