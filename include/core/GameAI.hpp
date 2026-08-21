#pragma once
#include <atomic>
#include <vector>

class Controller;
class Player;
class Character;

class GameAI
{
public:
    GameAI() = default;

    GameAI(const GameAI& other) { *this = other; }
    GameAI& operator=(const GameAI& other);

    enum class Decision
    {
        Generic,
        ActionChoice,
        FighterSelect,
        MoveDestination,
        AttackTarget,
        CardSelect,
        BoostChoice
    };
    enum class CardPurpose
    {
        Attack,
        Defense,
        Boost,
        Scheme
    };

    Decision decisionKind = Decision::Generic;
    CardPurpose cardPurpose = CardPurpose::Attack;
    std::vector<Character*> characterOptions;
    Character* movingCharacter = nullptr;
    Character* cardFighter = nullptr;

    int choose(Controller& controller, const std::vector<int>& valid, Player* decider);
    bool yesNo(Controller& controller, Player* decider);
    int chooseInt(Controller& controller, Player* decider);

    bool guiStartTurn(Controller& controller);
    bool guiTurnBusy() const;

private:
    std::atomic<bool> turnBusy{false};

    int scoreAction(Controller& controller, int action, Player* decider);
    int scoreFighter(int idx, Player* decider);
    int scoreMove(Controller& controller, int destination, Player* decider);
    int scoreAttackTarget(int idx, Player* decider);
    int scoreCardChoice(int idx, Player* decider);
    void think(Player* decider);

    void runTurn(Controller& controller);
};
