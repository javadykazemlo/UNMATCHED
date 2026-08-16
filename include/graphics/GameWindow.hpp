#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include "core/Controller.hpp"
#include "graphics/TextureManager.hpp"
#include "graphics/AudioManager.hpp"

class BoardView;
class CharacterView;
class CardView;
class DeckView;
class UI;
class RulesView;

class GameWindow
{
public:
    GameWindow();
    ~GameWindow();
    void run();

private:
    enum class Screen { MainMenu, LoadGame, Setup, Game, GameOver };

    sf::RenderWindow window;
    sf::Font font;
    TextureManager textures;
    AudioManager audio;
    Controller controller;
    Player players[2];

    std::unique_ptr<BoardView> boardView;
    std::unique_ptr<CharacterView> characterView;
    std::unique_ptr<CardView> cardView;
    std::unique_ptr<DeckView> deckView;
    std::unique_ptr<UI> ui;
    std::unique_ptr<RulesView> rulesView;

    Screen screen = Screen::MainMenu;
    int selectedHero1 = 1;
    int selectedHero2 = 2;
    int selectedSpace = -1;
    int selectedCharacter = -1;
    int selectedCard = -1;
    int selectedEnemy = -1;
    bool player2AI = true;
    std::string player1Name = "PLAYER 1";
    std::string player2Name = "PLAYER 2";
    std::string player1Age;
    std::string player2Age;
    int activeInputField = 0; // 0=name1, 1=age1, 2=name2, 3=age2
    bool setupStarted = false;
    bool attackMode = false;
    bool moveMode = false;
    bool boostMode = false;
    bool schemeMode = false;
    bool defenseSelectionMode = false;
    // After an attack target is chosen, the defender must explicitly decide
    // whether to play a defense card. Only after YES do we enter card selection.
    bool awaitingDefenseDecision = false;
    bool awaitingMoveBoost = false;
    bool moveBoostPrompt = false;
    // Which of the three fully-independent effect panels (if any) currently
    // owns the screen. Only one can be active at a time, since only one
    // effect can be resolving on the Controller side at once.
    enum class EffectPanelKind { None, Attack, Scheme, Dracula };
    EffectPanelKind activeEffectPanel = EffectPanelKind::None;

    // --- Attack effects panel state (combat resolution) ---
    std::string attackCardName;
    std::string attackCardText;
    std::string attackPrompt;
    std::vector<int> attackChoices;
    bool attackYesNo = false;
    bool attackInteger = false;
    bool attackResolved = false;
    std::string attackInputBuffer;

    // --- Scheme effects panel state ---
    std::string schemeCardName;
    std::string schemeCardText;
    std::string schemePrompt;
    std::vector<int> schemeChoices;
    bool schemeYesNo = false;
    bool schemeInteger = false;
    bool schemeResolved = false;
    std::string schemeInputBuffer;

    // --- Dracula special-ability panel state ---
    std::string draculaPrompt;
    std::vector<int> draculaChoices;
    bool draculaYesNo = false;
    bool draculaInteger = false;
    bool draculaResolved = false;
    std::string draculaInputBuffer;

    bool handLimitMode = false;

    struct SaveEntry
    {
        std::string path;
        std::string date;
        std::string time;
        std::string player1;
        std::string player2;
        std::string heroes;
        long long timestamp = 0;
    };

    std::vector<SaveEntry> saveEntries;
    float loadScroll = 0.f;
    bool draculaAbilityPrompt = false;
    std::string effectInputBuffer;
    int moveBoost = 0;
    std::string message;
    int messageTimer = 0;
    std::vector<std::string> combatLog;
    std::string winnerName;

    void processEvents();
    void update();
    void render();
    void drawMainMenu();
    void drawSetup();
    void drawLoadGame();
    void drawGame();
    void drawGameOver();
    void handleMainMenuClick(sf::Vector2f p);
    void handleLoadGameClick(sf::Vector2f p);
    void handleSetupClick(sf::Vector2f p);
    void refreshSaveEntries();
    void handleGameClick(sf::Vector2f p);
    void startGame();
    void showMessage(const std::string& text);
    void loadAssets();
    void loadCharacterAssets();
    void loadCardAssets();
    void drawFullscreenTexture(const std::string& id);
    void drawSetupPlayerInfo();
    void drawSetupCharacters();
    void drawSetupPosition();
    void drawSetupSidekicks();
    void drawSetupReady();
    void resetSelections();
    Character* selectedCurrentCharacter() const;
    void checkGameOver();
    void refreshCombatLog();

    // Each of the three effect panels below is fully self-contained: its own
    // update (polling the Controller for input requests / completion), its
    // own render, and its own click handling. None of them share logic or
    // state with the others.
    void updateAttackEffectPanel();
    void drawAttackEffectPanel();
    void handleAttackEffectInput(sf::Vector2f p);

    void updateSchemeEffectPanel();
    void drawSchemeEffectPanel();
    void handleSchemeEffectInput(sf::Vector2f p);

    void updateDraculaEffectPanel();
    void drawDraculaEffectPanel();
    void handleDraculaEffectInput(sf::Vector2f p);

    void advanceTurnIfTwoActionsUsed();
    void finishTurnAfterHandLimit();
    void completeCurrentAction();
    void openNextTurnIfReady();
};