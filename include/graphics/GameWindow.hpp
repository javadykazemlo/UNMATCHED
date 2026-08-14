#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include "core/Controller.hpp"
#include "graphics/TextureManager.hpp"

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
    enum class Screen { MainMenu, Setup, Game, GameOver };

    sf::RenderWindow window;
    sf::Font font;
    TextureManager textures;
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
    bool awaitingMoveBoost = false;
    bool moveBoostPrompt = false;
    bool effectPanelActive = false;
    std::string effectCardName;
    std::string effectCardText;
    std::string effectPrompt;
    std::vector<int> effectChoices;
    bool effectYesNo = false;
    bool effectInteger = false;
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
    void drawGame();
    void drawGameOver();
    void handleMainMenuClick(sf::Vector2f p);
    void handleSetupClick(sf::Vector2f p);
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
    void updateEffectPanel();
    void drawEffectPanel();
    void handleEffectInput(sf::Vector2f p);
};