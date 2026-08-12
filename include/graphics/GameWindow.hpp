#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <memory>
#include "core/Controller.hpp"

class BoardView;
class CharacterView;
class CardView;
class DeckView;
class UI;

class GameWindow
{
public:
    GameWindow();
    ~GameWindow();
    void run();

private:
    enum class Screen { MainMenu, Setup, Game };

    sf::RenderWindow window;
    sf::Font font;
    Controller controller;
    Player players[2];

    std::unique_ptr<BoardView> boardView;
    std::unique_ptr<CharacterView> characterView;
    std::unique_ptr<CardView> cardView;
    std::unique_ptr<DeckView> deckView;
    std::unique_ptr<UI> ui;

    Screen screen = Screen::MainMenu;
    int selectedHero1 = 1;
    int selectedHero2 = 2;
    int selectedSpace = -1;
    int selectedCharacter = -1;
    int selectedCard = -1;
    int selectedEnemy = -1;
    bool player2AI = true;
    bool attackMode = false;
    int messageTimer = 0;
    std::string message;

    void processEvents();
    void update();
    void render();

    void drawMainMenu();
    void drawSetup();
    void drawGame();

    void handleMainMenuClick(sf::Vector2f p);
    void handleSetupClick(sf::Vector2f p);
    void handleGameClick(sf::Vector2f p);

    void startGame();
    Character* characterAtScreen(sf::Vector2f p);
};
