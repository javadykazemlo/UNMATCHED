#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <string>
#include "core/Bord.hpp"
#include "core/Player.hpp"
#include "entities/Character.hpp"
#include "cards/Deck.hpp"
#include "cards/Card.hpp"

class GameWindow;

class Controller
{
public:
    friend class GameWindow;
    enum class GuiSetupStage
    {
        PlayerInfo,
        CharacterSelection,
        HeroPosition,
        SidekickPlacement,
        Ready
    };

private:
    Bord bord;

    Player* current = nullptr;
    Player* enemy = nullptr;

    bool cancelEffectDR = false;
    bool cancelEffectSH = false;
    bool cancelEffectIM = false;
    
    int gamerand = 0;
    bool GuessElementary = false;
    bool guiMode = false;
    std::vector<std::string> guiCombatLog;

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


    GuiSetupStage guiSetupStage = GuiSetupStage::PlayerInfo;
    Player* guiPlayers = nullptr;
    int guiCharacterPlayerIndex = -1;
    int guiPositionPlayerIndex = -1;
    int guiSidekickPlayerIndex = -1;
    int guiSidekickIndex = 1;
    int guiSidekicksDonePlayers = 0;

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

    Controller() = default;
    
    void startMenu(Player player[2]);
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
    Player* getCurrentPlayer() const;
    Player* getEnemyPlayer();
    Character* getCharacterAt(int position);
    bool isGameOver();
    
    void SaveGame(const std::string& filename = "save.json");
    bool LoadGame(Player player[2], const std::string& filename = "save.json");
    bool end_game() const;

    void applyEffect(Card& card, Card& enemycard, Player* self, Player* opponent, Character* attacker, Character* defender, bool woncombat);
    void applyEffectScheme(Card& card, Player* self, Player* opponent, Character* attacker);

    bool beginGuiSetup(Player players[2]);
    bool guiFinishPlayerSetup(const std::string& player1Name, int player1Age,
                              const std::string& player2Name, int player2Age,
                              bool player2AI);
    GuiSetupStage getGuiSetupStage() const;
    Player* getGuiSetupPlayer() const;
    std::vector<int> getGuiCharacterChoices() const;
    std::vector<int> getGuiPlacementSpaces() const;
    bool guiChooseCharacter(int hero);
    bool guiChooseHeroPosition(int side);
    bool guiPlaceSidekick(int space);
    bool isGuiSetupReady() const;

    bool startGuiGame(Player players[2], int hero1, int hero2,
                      const std::string& player1Name = "Player 1",
                      const std::string& player2Name = "Player 2",
                      bool player2AI = false);
    std::vector<int> getValidMoveSpaces(Character* selected, int movement) const;
    bool guiMove(Character* selected, int movement, int destination);
    bool guiAttack(Character* attacker, Character* defender,
                   int attackCardIndex, int defenseCardIndex);
    bool guiUseBoostCard(int index, int& boostValue);
    std::vector<int> getGuiSchemeCards(Character* fighter) const;
    bool guiScheme(Character* fighter, int cardIndex);

    // Non-blocking GUI effect/input bridge. Card effects that need a choice
    // are executed in a worker thread and request their input from GameWindow.
    bool guiEffectBusy() const;
    bool guiEffectFinished();
    bool getGuiInputRequest(std::string& prompt, std::vector<int>& choices,
                            bool& yesNo, bool& integerInput) const;
    bool submitGuiInput(int value);
    bool submitGuiYesNo(bool value);

    bool guiSaveGame(const std::string& filename = "save.json");
    std::vector<std::string> getGuiCombatLog() const;
    void clearGuiCombatLog();
    std::string getGuiWinnerName() const;
    std::vector<int> getGuiAttackCards(Character* attacker) const;
    std::vector<int> getGuiDefenseCards(Character* defender) const;
    bool guiDrawCard();
    bool guiPlayCard(int index);
    int getActionCount() const;
    void guiEndAction();
    void guiEndTurn();
    bool isCurrentPlayer(const Character* character) const;

    ~Controller() = default;
};