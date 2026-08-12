#include "graphics/GameWindow.hpp"
#include "graphics/BoardView.hpp"
#include "graphics/CharacterView.hpp"
#include "graphics/CardView.hpp"
#include "graphics/DeckView.hpp"
#include "graphics/UI.hpp"
#include "entities/Character.hpp"
#include "cards/Deck.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace
{
    const sf::Color BG(7, 8, 13);
    const sf::Color GOLD(210, 177, 105);
    const sf::Color PARCHMENT(225, 216, 190);
    const sf::Color RED(143, 33, 36);
    const sf::Color BLUE(43, 85, 126);
}

GameWindow::~GameWindow() = default;

GameWindow::GameWindow()
    : window(sf::VideoMode({1600u, 900u}), "UNMATCHED - Dark Gothic Edition")
{
    window.setFramerateLimit(60);

    if (!font.openFromFile("assets/fonts/Cinzel-Bold.ttf"))
        throw std::runtime_error("Could not load assets/fonts/Cinzel-Bold.ttf");

    boardView = std::make_unique<BoardView>();
    characterView = std::make_unique<CharacterView>();
    cardView = std::make_unique<CardView>(font);
    deckView = std::make_unique<DeckView>();
    ui = std::make_unique<UI>(font);
}

void GameWindow::run()
{
    while (window.isOpen())
    {
        processEvents();
        update();

        window.clear(BG);
        render();
        window.display();
    }
}

void GameWindow::processEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            continue;
        }

        if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button != sf::Mouse::Button::Left)
                continue;

            sf::Vector2f p = window.mapPixelToCoords(mouse->position);

            switch (screen)
            {
                case Screen::MainMenu: handleMainMenuClick(p); break;
                case Screen::Setup:    handleSetupClick(p); break;
                case Screen::Game:     handleGameClick(p); break;
            }
        }
    }
}

void GameWindow::update()
{
    if (messageTimer > 0)
        --messageTimer;
}

void GameWindow::render()
{
    switch (screen)
    {
        case Screen::MainMenu: drawMainMenu(); break;
        case Screen::Setup:    drawSetup(); break;
        case Screen::Game:     drawGame(); break;
    }
}

void GameWindow::drawMainMenu()
{
    sf::RectangleShape bg({1600.f, 900.f});
    bg.setFillColor(sf::Color(5, 6, 10));
    window.draw(bg);

    // Procedural gothic city silhouette; no external background image is required.
    for (int i = 0; i < 11; ++i)
    {
        float x = i * 155.f - 40.f;
        float h = 260.f + (i % 4) * 70.f;

        sf::RectangleShape building({145.f, h});
        building.setPosition({x, 900.f - h});
        building.setFillColor(sf::Color(12, 13, 18));
        building.setOutlineColor(sf::Color(30, 30, 36));
        building.setOutlineThickness(1.f);
        window.draw(building);

        for (int y = static_cast<int>(900.f - h + 40); y < 850; y += 65)
        {
            sf::RectangleShape lamp({8.f, 20.f});
            lamp.setPosition({x + 25.f, static_cast<float>(y)});
            lamp.setFillColor(sf::Color(120, 96, 55, 70));
            window.draw(lamp);
        }
    }

    sf::CircleShape moon(90.f);
    moon.setPosition({1250.f, 100.f});
    moon.setFillColor(sf::Color(175, 169, 148, 18));
    window.draw(moon);

    sf::Text small(font, "THE ETERNAL BATTLE OF SHADOWS", 16);
    auto sb = small.getLocalBounds();
    small.setOrigin({sb.position.x + sb.size.x/2.f, sb.position.y + sb.size.y/2.f});
    small.setPosition({800.f, 275.f});
    small.setFillColor(sf::Color(157, 126, 69));
    window.draw(small);

    sf::Text title(font, "UNMATCHED", 64);
    auto tb = title.getLocalBounds();
    title.setOrigin({tb.position.x + tb.size.x/2.f, tb.position.y + tb.size.y/2.f});
    title.setPosition({800.f, 335.f});
    title.setFillColor(GOLD);
    window.draw(title);

    ui->drawButton(window, {{575.f, 425.f}, {450.f, 60.f}}, "START GAME", true, RED);
    ui->drawButton(window, {{575.f, 505.f}, {450.f, 60.f}}, "LOAD GAME", false, GOLD);
    ui->drawButton(window, {{575.f, 585.f}, {450.f, 60.f}}, "EXIT", false, GOLD);

    ui->drawText(window, "SFML 3.x  •  C++17", {690.f, 690.f}, 14,
                 sf::Color(125, 120, 112));
}

void GameWindow::drawSetup()
{
    sf::RectangleShape bg({1600.f, 900.f});
    bg.setFillColor(sf::Color(7, 8, 13));
    window.draw(bg);

    sf::Text title(font, "ASSESSMENT OF MORTALITY", 32);
    title.setPosition({555.f, 70.f});
    title.setFillColor(GOLD);
    window.draw(title);

    ui->drawPanel(window, {{360.f, 155.f}, {880.f, 620.f}}, GOLD);

    ui->drawText(window, "PLAYER 1", {430.f, 195.f}, 22, RED);
    ui->drawText(window, "PLAYER 2", {885.f, 195.f}, 22, BLUE);

    ui->drawText(window, "Choose your hero", {430.f, 250.f}, 16);
    ui->drawText(window, "Choose opponent hero", {885.f, 250.f}, 16);

    const char* heroes[] = {"DRACULA", "SHERLOCK", "INVISIBLE MAN"};

    for (int i = 0; i < 3; ++i)
    {
        sf::FloatRect r1({410.f, 295.f + i*72.f}, {350.f, 54.f});
        sf::FloatRect r2({865.f, 295.f + i*72.f}, {350.f, 54.f});

        ui->drawButton(window, r1, heroes[i], selectedHero1 == i+1, RED);
        ui->drawButton(window, r2, heroes[i], selectedHero2 == i+1, BLUE);
    }

    ui->drawButton(window, {{865.f, 535.f}, {350.f, 52.f}},
                   player2AI ? "PLAYER 2 : AI" : "PLAYER 2 : HUMAN",
                   player2AI, BLUE);

    ui->drawText(window,
                 "The GUI keeps the existing Bord adjacency and C++ game rules unchanged.",
                 {425.f, 620.f}, 13, sf::Color(154, 147, 135));

    ui->drawButton(window, {{490.f, 680.f}, {620.f, 58.f}},
                   "ENTER THE BOARD", true, GOLD);

    ui->drawText(window, "Choose two different heroes.", {620.f, 755.f}, 13,
                 sf::Color(150, 70, 70));
}

void GameWindow::drawGame()
{
    // Background
    sf::RectangleShape bg({1600.f, 900.f});
    bg.setFillColor(sf::Color(7, 8, 12));
    window.draw(bg);

    // Top bar
    sf::RectangleShape top({1600.f, 72.f});
    top.setFillColor(sf::Color(10, 10, 16));
    top.setOutlineColor(sf::Color(75, 62, 45));
    top.setOutlineThickness(1.f);
    window.draw(top);

    sf::Text logo(font, "UNMATCHED", 28);
    logo.setPosition({25.f, 18.f});
    logo.setFillColor(GOLD);
    window.draw(logo);

    Player* current = controller.getCurrentPlayer();
    Player* enemy = controller.getEnemyPlayer();

    std::string turn = current ? current->getName() + "'S TURN" : "PLAYER TURN";
    ui->drawText(window, turn, {660.f, 22.f}, 20, current && current->getHero()->getowner() == 1 ? RED : BLUE);

    ui->drawText(window, "HERO PHASE", {775.f, 48.f}, 11, sf::Color(145, 138, 126));

    ui->drawButton(window, {{1370.f, 15.f}, {92.f, 42.f}}, "RULES", false, GOLD);
    ui->drawButton(window, {{1470.f, 15.f}, {105.f, 42.f}}, "EXIT", false, GOLD);

    // Player panels
    auto drawPlayerPanel = [&](Player* player, sf::FloatRect rect, sf::Color accent)
    {
        ui->drawPanel(window, rect, accent);

        if (!player || player->getCharacters().empty())
            return;

        Character* hero = player->getHero();

        ui->drawText(window, player->getName(), {rect.position.x + 22.f, rect.position.y + 18.f}, 20, accent);
        ui->drawText(window, hero->getName(), {rect.position.x + 22.f, rect.position.y + 52.f}, 16, PARCHMENT);

        std::string hp = std::to_string(hero->getHp()) + "/" + std::to_string(hero->getMaxhp());
        ui->drawText(window, hp, {rect.position.x + 22.f, rect.position.y + 82.f}, 14, PARCHMENT);
        ui->drawHealth(window, {rect.position.x + 22.f, rect.position.y + 112.f},
                       static_cast<float>(hero->getHp()) / std::max(1, hero->getMaxhp()),
                       rect.size.x - 44.f, accent);

        float y = rect.position.y + 155.f;
        for (Character* c : player->getCharacters())
        {
            if (!c || c == hero) continue;

            sf::CircleShape token(19.f);
            token.setOrigin({19.f,19.f});
            token.setPosition({rect.position.x + 45.f, y + 5.f});
            token.setFillColor(accent);
            token.setOutlineColor(sf::Color(210, 194, 160));
            token.setOutlineThickness(1.f);
            window.draw(token);

            ui->drawText(window, c->getName(), {rect.position.x + 72.f, y - 7.f}, 12,
                         c->checkalive() ? PARCHMENT : sf::Color(100,95,90));
            ui->drawText(window, std::to_string(c->getHp()) + "/" + std::to_string(c->getMaxhp()),
                         {rect.position.x + 72.f, y + 12.f}, 10, sf::Color(150,145,137));
            y += 48.f;
        }
    };

    drawPlayerPanel(current, {{20.f, 95.f}, {475.f, 540.f}}, RED);
    drawPlayerPanel(enemy, {{1140.f, 95.f}, {440.f, 540.f}}, BLUE);

    // Board
    std::vector<int> highlights;
    if (selectedCharacter >= 0 && current)
    {
        Character* c = current->getFighter(selectedCharacter);
        if (c)
            highlights = controller.getValidMoveSpaces(c, c->getMove());
    }

    boardView->draw(window, controller.getBord(), selectedSpace, highlights);

    // Characters on the board
    for (int i = 0; i < 32; ++i)
    {
        Character* c = controller.getCharacterAt(i);
        if (!c) continue;

        bool selected = c->getowner() == 1
            ? (selectedCharacter >= 0 && current && current->getFighter(selectedCharacter) == c)
            : (selectedEnemy == i);

        characterView->draw(window, c, boardView->getPosition(i), selected);
    }

    // Bottom action area
    ui->drawPanel(window, {{20.f, 655.f}, {475.f, 225.f}}, RED);
    ui->drawText(window, "ACTIONS", {40.f, 675.f}, 18, RED);
    ui->drawText(window, "Select a fighter, then choose a destination.", {40.f, 704.f}, 11,
                 sf::Color(145, 140, 132));

    ui->drawButton(window, {{40.f, 740.f}, {190.f, 48.f}}, "MOVE", true, sf::Color(70, 135, 85));
    ui->drawButton(window, {{250.f, 740.f}, {190.f, 48.f}}, "ATTACK", attackMode, RED);
    ui->drawButton(window, {{40.f, 800.f}, {190.f, 48.f}}, "SCHEME", false, sf::Color(100, 70, 125));
    ui->drawButton(window, {{250.f, 800.f}, {190.f, 48.f}}, "END ACTION", false, GOLD);

    // Hand
    ui->drawPanel(window, {{510.f, 655.f}, {625.f, 225.f}}, GOLD);
    ui->drawText(window, "YOUR HAND", {535.f, 675.f}, 18, GOLD);

    if (current && current->getDeck())
        cardView->drawHand(window, current->getDeck()->gethand(), selectedCard);

    // Turn panel
    ui->drawPanel(window, {{1155.f, 655.f}, {425.f, 225.f}}, BLUE);
    ui->drawText(window, "TURN ORDER", {1180.f, 675.f}, 18, BLUE);
    ui->drawText(window, current ? current->getName() : "-", {1180.f, 720.f}, 17, PARCHMENT);
    ui->drawText(window, "ACTION " + std::to_string(controller.getActionCount()+1) + " / 2",
                 {1180.f, 752.f}, 13, sf::Color(160, 153, 145));

    if (current && current->getDeck())
    {
        deckView->draw(window, {1180.f, 790.f},
                       current->getDeck()->getdeckSize(),
                       current->getDeck()->getdiscardSize());

        ui->drawText(window, "DECK " + std::to_string(current->getDeck()->getdeckSize()),
                     {1265.f, 800.f}, 12, PARCHMENT);
    }

    ui->drawButton(window, {{1380.f, 800.f}, {175.f, 48.f}}, "END TURN", true, BLUE);

    if (messageTimer > 0)
        ui->drawText(window, message, {540.f, 625.f}, 12, sf::Color(220, 185, 100));
}

void GameWindow::handleMainMenuClick(sf::Vector2f p)
{
    if (sf::FloatRect({575.f,425.f},{450.f,60.f}).contains(p))
    {
        screen = Screen::Setup;
        return;
    }

    if (sf::FloatRect({575.f,585.f},{450.f,60.f}).contains(p))
    {
        window.close();
    }
}

void GameWindow::handleSetupClick(sf::Vector2f p)
{
    for (int i = 0; i < 3; ++i)
    {
        if (sf::FloatRect({410.f, 295.f + i*72.f},{350.f,54.f}).contains(p))
            selectedHero1 = i+1;

        if (sf::FloatRect({865.f, 295.f + i*72.f},{350.f,54.f}).contains(p))
            selectedHero2 = i+1;
    }

    if (sf::FloatRect({865.f,535.f},{350.f,52.f}).contains(p))
    {
        player2AI = !player2AI;
        return;
    }

    if (sf::FloatRect({490.f,680.f},{620.f,58.f}).contains(p))
    {
        if (selectedHero1 == selectedHero2)
        {
            message = "Choose two different heroes.";
            messageTimer = 180;
            return;
        }
        startGame();
    }
}

void GameWindow::handleGameClick(sf::Vector2f p)
{
    // Exit button
    if (sf::FloatRect({1470.f,15.f},{105.f,42.f}).contains(p))
    {
        window.close();
        return;
    }

    // End turn
    if (sf::FloatRect({1380.f,800.f},{175.f,48.f}).contains(p))
    {
        controller.guiEndTurn();
        selectedCharacter = -1;
        selectedSpace = -1;
        selectedEnemy = -1;
        selectedCard = -1;
        attackMode = false;
        return;
    }

    // Move
    if (sf::FloatRect({40.f,740.f},{190.f,48.f}).contains(p))
    {
        attackMode = false;
        message = "MOVE: select one of your fighters, then click a green board space.";
        messageTimer = 240;
        return;
    }

    // Attack
    if (sf::FloatRect({250.f,740.f},{190.f,48.f}).contains(p))
    {
        if (selectedCharacter >= 0 && selectedEnemy >= 0 && selectedCard >= 0)
        {
            Player* cp = controller.getCurrentPlayer();
            Character* attacker = cp ? cp->getFighter(selectedCharacter) : nullptr;
            Character* defender = controller.getCharacterAt(selectedEnemy);
            Player* ep = controller.getEnemyPlayer();
            int defenseIndex = -1;

            if (ep && ep->getDeck() && defender)
            {
                for (int i = 0; i < ep->getDeck()->gethandSize(); ++i)
                {
                    Card c = ep->getDeck()->getHandcard(i);
                    bool ownerOK = defender->isHero()
                        ? (c.isDefense() || c.isVersatile()) && (c.isHero() || c.isAnyowner())
                        : (c.isDefense() || c.isVersatile()) && (c.issideKick() || c.isAnyowner());

                    if (ownerOK) { defenseIndex = i; break; }
                }
            }

            if (attacker && defender && defenseIndex >= 0 &&
                controller.guiAttack(attacker, defender, selectedCard, defenseIndex))
            {
                controller.guiEndAction();
                message = "Combat resolved.";
                messageTimer = 150;
                selectedCard = -1;
                selectedEnemy = -1;

                if (controller.getActionCount() >= 2)
                {
                    controller.guiEndTurn();
                    selectedCharacter = -1;
                }
                return;
            }

            message = "Invalid attack/card/target selection.";
            messageTimer = 180;
            return;
        }

        attackMode = true;
        message = "ATTACK: select attacker, enemy, attack card, then press ATTACK.";
        messageTimer = 240;
        return;
    }

    // End action
    if (sf::FloatRect({250.f,800.f},{190.f,48.f}).contains(p))
    {
        controller.guiEndAction();
        selectedCharacter = -1;
        selectedSpace = -1;
        selectedEnemy = -1;
        selectedCard = -1;
        attackMode = false;

        if (controller.getActionCount() >= 2)
            controller.guiEndTurn();
        return;
    }

    // Card selection
    Player* current = controller.getCurrentPlayer();
    if (current && current->getDeck())
    {
        int c = cardView->getCardAt(p, current->getDeck()->gethandSize());
        if (c >= 0)
        {
            selectedCard = c;
            return;
        }
    }

    // Board selection
    int space = boardView->getSpaceAt(p);
    if (space >= 0)
    {
        Character* occupant = controller.getCharacterAt(space);

        if (occupant && controller.isCurrentPlayer(occupant))
        {
            selectedSpace = space;
            Player* currentPlayer = controller.getCurrentPlayer();
            for (int i = 0; i < currentPlayer->getfighterCount(); ++i)
            {
                if (currentPlayer->getFighter(i) == occupant)
                {
                    selectedCharacter = i;
                    break;
                }
            }
            return;
        }

        if (attackMode && occupant && !controller.isCurrentPlayer(occupant))
        {
            selectedEnemy = space;
            return;
        }

        if (!attackMode && selectedCharacter >= 0)
        {
            Player* cp = controller.getCurrentPlayer();
            Character* mover = cp ? cp->getFighter(selectedCharacter) : nullptr;

            if (mover)
            {
                if (controller.guiMove(mover, mover->getMove(), space))
                {
                    controller.guiEndAction();
                    message = "Movement completed.";
                    messageTimer = 120;

                    selectedSpace = space;
                    if (controller.getActionCount() >= 2)
                    {
                        controller.guiEndTurn();
                        selectedCharacter = -1;
                        selectedSpace = -1;
                    }
                }
                else
                {
                    message = "That destination is not legal for this movement.";
                    messageTimer = 180;
                }
            }
        }
        return;
    }
}

void GameWindow::startGame()
{
    if (!controller.startGuiGame(players, selectedHero1, selectedHero2,
                                 "Player 1", player2AI ? "AI" : "Player 2",
                                 player2AI))
    {
        message = "Could not start the game.";
        messageTimer = 180;
        return;
    }

    screen = Screen::Game;
    selectedCharacter = -1;
    selectedSpace = -1;
    selectedEnemy = -1;
    selectedCard = -1;
    attackMode = false;
}

Character* GameWindow::characterAtScreen(sf::Vector2f p)
{
    int space = boardView->getSpaceAt(p);
    return space >= 0 ? controller.getCharacterAt(space) : nullptr;
}
