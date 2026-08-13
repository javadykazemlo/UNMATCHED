#include "graphics/GameWindow.hpp"
#include "graphics/BoardView.hpp"
#include "graphics/CharacterView.hpp"
#include "graphics/CardView.hpp"
#include "graphics/DeckView.hpp"
#include "graphics/UI.hpp"
#include "entities/Character.hpp"
#include "entities/invisible_man.hpp"
#include "cards/Deck.hpp"
#include "cards/Card.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <optional>
#include <cmath>
#include <stdexcept>
#include <cctype>

namespace
{
    const sf::Color BG(6, 7, 11);
    const sf::Color GOLD(211, 178, 104);
    const sf::Color PARCHMENT(226, 216, 190);
    const sf::Color RED(145, 35, 39);
    const sf::Color BLUE(40, 82, 123);
    const sf::Color GREEN(62, 121, 77);
    const sf::Color PURPLE(91, 61, 118);
}

GameWindow::GameWindow()
    : window(sf::VideoMode({1600u, 900u}), "UNMATCHED - Dark Gothic Edition")
{
    window.setFramerateLimit(60);

    if (!font.openFromFile("assets/fonts/Cinzel-Bold.ttf"))
        throw std::runtime_error("Could not load assets/fonts/Cinzel-Bold.ttf");

    loadAssets();
    boardView = std::make_unique<BoardView>(&textures);
    characterView = std::make_unique<CharacterView>(font, textures);
    cardView = std::make_unique<CardView>(font, &textures);
    deckView = std::make_unique<DeckView>(font, &textures);
    ui = std::make_unique<UI>(font);
}

GameWindow::~GameWindow() = default;

void GameWindow::loadAssets()
{
    textures.load("main_menu", "assets/backgrounds/main_menu.png");
    textures.load("setup", "assets/backgrounds/setup.png");
    textures.load("game", "assets/backgrounds/game.png");
    textures.load("board", "assets/board/board.png");
    textures.load("card_back", "assets/cards/card_back.png");

    loadCharacterAssets();
    loadCardAssets();
}

void GameWindow::loadCardAssets()
{
    const std::vector<std::string> names = {
        "Feeding Frenzy", "MistForm", "Ambush", "Baptism of Blood",
        "BeastForm", "Dash", "Exploit", "Look Into My Eyes", "Prey Upon",
        "Ravening Seduction", "Thirst for Sustenance", "Feint",
        "Administer Aid", "Confirm Suspicion", "Counterpunch", "Deduce Strategy",
        "Education Never Ends", "Elementary", "Eliminate the Impossible",
        "Fixed Point in a Changing Age", "Master of Disguise", "The Game is Afoot",
        "Service Revolver", "Study Methods", "Coded Notes", "Confound",
        "Covert Preparation", "Dreaming of Revenge", "Emerge from Mist",
        "Impossible to See", "Into Thin Air", "Lurking", "Reign of Terror",
        "Rolling Fog", "Slip Away", "Step Lightly", "Vanish"
    };

    for (const std::string& name : names)
    {
        std::string id = "card_";
        for (char c : name)
        {
            if (c >= 'A' && c <= 'Z') id += static_cast<char>(c - 'A' + 'a');
            else if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) id += c;
            else id += '_';
        }
        while (!id.empty() && id.back() == '_') id.pop_back();

        std::string filename = id.substr(5) + ".png";
        textures.load(id, "assets/cards/" + filename);
    }
}

void GameWindow::loadCharacterAssets()
{
    textures.load("dracula", "assets/characters/dracula.png");
    textures.load("sherlock", "assets/characters/sherlock.png");
    textures.load("watson", "assets/characters/watson.png");
    textures.load("sisters", "assets/characters/sisters.png");
    textures.load("invisible_man", "assets/characters/invisible_man.png");
}

void GameWindow::drawFullscreenTexture(const std::string& id)
{
    if (const sf::Texture* texture = textures.get(id))
    {
        sf::Sprite sprite(*texture);
        const sf::Vector2u size = texture->getSize();
        if (size.x > 0 && size.y > 0)
        {
            sprite.setScale({1600.f / static_cast<float>(size.x),
                             900.f / static_cast<float>(size.y)});
            sprite.setPosition({0.f, 0.f});
            window.draw(sprite);
            return;
        }
    }
}

    sf::RectangleShape fallback({1600.f, 900.f});
    fallback.setFillColor(BG);
    window.draw(fallback);
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

        if (screen == Screen::Setup)
        {
            if (const auto* text = event->getIf<sf::Event::TextEntered>())
            {
                if (activeInputField == 0 || activeInputField == 2)
                {
                    if (text->unicode >= 32 && text->unicode < 127)
                    {
                        std::string* value = activeInputField == 0 ? &player1Name : &player2Name;
                        if (value->size() < 18)
                            value->push_back(static_cast<char>(text->unicode));
                    }
                }
                else if (text->unicode >= '0' && text->unicode <= '9')
                {
                    std::string* value = activeInputField == 1 ? &player1Age : &player2Age;
                    if (value->size() < 3)
                        value->push_back(static_cast<char>(text->unicode));
                }
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Backspace)
                {
                    std::string* value = nullptr;
                    if (activeInputField == 0) value = &player1Name;
                    else if (activeInputField == 1) value = &player1Age;
                    else if (activeInputField == 2) value = &player2Name;
                    else value = &player2Age;
                    if (value && !value->empty()) value->pop_back();
                }
                else if (key->code == sf::Keyboard::Key::Tab)
                {
                    activeInputField = (activeInputField + 1) % 4;
                }
            }
        }

        if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button != sf::Mouse::Button::Left) continue;
            const sf::Vector2f p = window.mapPixelToCoords(mouse->position);

            if (screen == Screen::MainMenu) handleMainMenuClick(p);
            else if (screen == Screen::Setup) handleSetupClick(p);
            else handleGameClick(p);
        }
    }
}

void GameWindow::update()
{
    if (messageTimer > 0) --messageTimer;
}

void GameWindow::render()
{
    if (screen == Screen::MainMenu) drawMainMenu();
    else if (screen == Screen::Setup) drawSetup();
    else drawGame();
}

void GameWindow::showMessage(const std::string& text)
{
    message = text;
    messageTimer = 240;
}

void GameWindow::drawMainMenu()
{
    drawFullscreenTexture("main_menu");

    sf::RectangleShape overlay({1600.f, 900.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 85));
    window.draw(overlay);

    ui->drawText(window, "THE ETERNAL BATTLE OF SHADOWS", {670.f, 270.f}, 14,
                 sf::Color(157, 126, 69));
    ui->drawText(window, "UNMATCHED", {600.f, 305.f}, 58, GOLD);

    ui->drawButton(window, {{575.f, 425.f}, {450.f, 60.f}}, "START GAME", true, RED);
    ui->drawButton(window, {{575.f, 505.f}, {450.f, 60.f}}, "LOAD GAME", false, GOLD);
    ui->drawButton(window, {{575.f, 585.f}, {450.f, 60.f}}, "EXIT", false, GOLD);
}

void GameWindow::drawSetup()
{
    drawFullscreenTexture("setup");

    if (!setupStarted)
    {
        drawSetupPlayerInfo();
        return;
    }

    switch (controller.getGuiSetupStage())
    {
        case Controller::GuiSetupStage::PlayerInfo:
            drawSetupPlayerInfo();
            break;
        case Controller::GuiSetupStage::CharacterSelection:
            drawSetupCharacters();
            break;
        case Controller::GuiSetupStage::HeroPosition:
            drawSetupPosition();
            break;
        case Controller::GuiSetupStage::SidekickPlacement:
            drawSetupSidekicks();
            break;
        case Controller::GuiSetupStage::Ready:
            drawSetupReady();
            break;
    }
}

void GameWindow::drawSetupPlayerInfo()
{
    ui->drawText(window, "THE PLAYERS", {640.f, 62.f}, 32, GOLD);
    ui->drawPanel(window, {{245.f, 120.f}, {1110.f, 650.f}}, GOLD);

    auto field = [&](sf::FloatRect rect, const std::string& label,
                     const std::string& value, bool active)
    {
        ui->drawText(window, label, {rect.position.x, rect.position.y - 25.f}, 12, PARCHMENT);
        ui->drawPanel(window, rect, active ? GOLD : sf::Color(80, 73, 62));
        ui->drawText(window, value.empty() ? "_" : value,
                     {rect.position.x + 15.f, rect.position.y + 12.f}, 17,
                     active ? GOLD : PARCHMENT);
    };

    ui->drawText(window, "PLAYER 1", {350.f, 170.f}, 22, RED);
    field({{350.f, 225.f}, {380.f, 50.f}}, "NAME", player1Name, activeInputField == 0);
    field({{350.f, 315.f}, {180.f, 50.f}}, "AGE", player1Age, activeInputField == 1);

    ui->drawText(window, "PLAYER 2", {900.f, 170.f}, 22, BLUE);
    field({{900.f, 225.f}, {380.f, 50.f}}, "NAME", player2AI ? "AI" : player2Name,
          !player2AI && activeInputField == 2);
    field({{900.f, 315.f}, {180.f, 50.f}}, "AGE", player2AI ? player1Age : player2Age,
          !player2AI && activeInputField == 3);

    ui->drawButton(window, {{900.f, 410.f}, {380.f, 52.f}},
                   player2AI ? "PLAYER 2 : AI" : "PLAYER 2 : HUMAN",
                   player2AI, BLUE);

    ui->drawText(window,
        "TAB: next field   |   Backspace: delete   |   Enter the player data first",
        {380.f, 555.f}, 11, sf::Color(165, 157, 145));

    ui->drawButton(window, {{520.f, 655.f}, {560.f, 62.f}},
                   "CONTINUE TO CHARACTER SELECTION", true, GOLD);

    if (messageTimer > 0)
        ui->drawText(window, message, {420.f, 600.f}, 11, RED);
}

void GameWindow::drawSetupCharacters()
{
    drawFullscreenTexture("setup");
    ui->drawText(window, "CHOOSE YOUR CHARACTERS", {535.f, 65.f}, 30, GOLD);

    Player* chooser = controller.getGuiSetupPlayer();
    Player* current = controller.getCurrentPlayer();
    Player* enemy = controller.getEnemyPlayer();

    ui->drawPanel(window, {{300.f, 125.f}, {1000.f, 640.f}}, GOLD);
    if (chooser)
    {
        ui->drawText(window, chooser->getName() + " - YOUR CHOICE",
                     {590.f, 165.f}, 18,
                     chooser == current ? RED : BLUE);
    }

    const int heroes[] = {1, 2, 3};
    const char* names[] = {"DRACULA", "SHERLOCK", "INVISIBLE MAN"};
    const char* portraits[] = {"dracula", "sherlock", "invisible_man"};

    const std::vector<int> choices = controller.getGuiCharacterChoices();
    for (int i = 0; i < 3; ++i)
    {
        const bool allowed = std::find(choices.begin(), choices.end(), heroes[i]) != choices.end();
        sf::FloatRect rect({430.f, 225.f + i * 135.f}, {740.f, 105.f});
        ui->drawButton(window, rect, names[i], allowed, allowed ? GOLD : sf::Color(70, 65, 58));

        if (allowed)
        {
            if (const sf::Texture* tex = textures.get(portraits[i]))
            {
                sf::Sprite sprite(*tex);
                const sf::Vector2u size = tex->getSize();
                const float scale = std::min(75.f / static_cast<float>(size.x),
                                             75.f / static_cast<float>(size.y));
                sprite.setScale({scale, scale});
                sprite.setPosition({rect.position.x + 15.f, rect.position.y + 15.f});
                window.draw(sprite);
            }
        }
    }

    ui->drawText(window, current ? "The older player chooses first." : "",
                 {555.f, 650.f}, 11, sf::Color(165, 157, 145));
}

void GameWindow::drawSetupPosition()
{
    drawFullscreenTexture("setup");
    ui->drawText(window, "CHOOSE STARTING SIDE", {540.f, 70.f}, 30, GOLD);

    Player* chooser = controller.getGuiSetupPlayer();
    ui->drawPanel(window, {{250.f, 140.f}, {1100.f, 610.f}}, GOLD);

    if (chooser)
        ui->drawText(window, chooser->getName() + " chooses the starting side.",
                     {555.f, 175.f}, 17, PARCHMENT);

    ui->drawButton(window, {{350.f, 270.f}, {390.f, 260.f}},
                   "LEFT", true, RED);
    ui->drawButton(window, {{860.f, 270.f}, {390.f, 260.f}},
                   "RIGHT", true, BLUE);

    ui->drawText(window, "Your hero starts on space 4.", {430.f, 555.f}, 12, PARCHMENT);
    ui->drawText(window, "Your opponent starts on space 15.", {430.f, 585.f}, 12, PARCHMENT);
}

void GameWindow::drawSetupSidekicks()
{
    drawFullscreenTexture("setup");

    Player* player = controller.getGuiSetupPlayer();
    ui->drawText(window, "PLACE YOUR FIGHTERS", {600.f, 40.f}, 30, GOLD);

    if (!player) return;

    ui->drawPanel(window, {{245.f, 705.f}, {1110.f, 120.f}}, GOLD);
    ui->drawText(window, player->getName() + " - choose a starting space",
                 {510.f, 745.f}, 17, PARCHMENT);

    ui->drawText(window,
        "Only spaces in your hero's starting zone are legal.",
        {510.f, 778.f}, 11, sf::Color(165, 157, 145));

    std::vector<int> valid = controller.getGuiPlacementSpaces();
    boardView->draw(window, controller.getBord(), -1, valid);

    for (int pos : valid)
    {
        const sf::Vector2f p = boardView->getPosition(pos);
        ui->drawText(window, "PLACE", {p.x - 20.f, p.y - 43.f}, 7, GOLD);
    }
}

void GameWindow::drawSetupReady()
{
    drawFullscreenTexture("setup");
    ui->drawText(window, "THE BATTLE IS READY", {590.f, 150.f}, 34, GOLD);
    ui->drawPanel(window, {{360.f, 245.f}, {880.f, 360.f}}, GOLD);

    ui->drawText(window, "Players, characters and starting positions are set.",
                 {505.f, 315.f}, 15, PARCHMENT);
    ui->drawText(window, "The game board is now controlled by the real Controller.",
                 {490.f, 350.f}, 13, sf::Color(170, 162, 150));

    ui->drawButton(window, {{520.f, 470.f}, {560.f, 62.f}},
                   "ENTER THE BATTLE", true, GOLD);
}

void GameWindow::drawGame()
{
    drawFullscreenTexture("game");

    sf::RectangleShape bgOverlay({1600.f, 900.f});
    bgOverlay.setFillColor(sf::Color(0, 0, 0, 75));
    window.draw(bgOverlay);

    sf::RectangleShape top({1600.f, 68.f});
    top.setFillColor(sf::Color(10, 10, 16));
    top.setOutlineColor(sf::Color(75, 62, 45));
    top.setOutlineThickness(1.f);
    window.draw(top);

    ui->drawText(window, "UNMATCHED", {22.f, 16.f}, 26, GOLD);
    Player* current = controller.getCurrentPlayer();
    Player* enemy = controller.getEnemyPlayer();
    const sf::Color turnColor = current && current->getHero()->getowner() == 1 ? RED : BLUE;
    ui->drawText(window, current ? current->getName() + "'S TURN" : "PLAYER TURN",
                 {635.f, 17.f}, 18, turnColor);
    ui->drawText(window, "HERO PHASE", {760.f, 43.f}, 10, sf::Color(150, 143, 132));
    ui->drawButton(window, {{1375.f, 12.f}, {90.f, 42.f}}, "RULES", false, GOLD);
    ui->drawButton(window, {{1472.f, 12.f}, {105.f, 42.f}}, "EXIT", false, GOLD);

    // Slightly smaller side panels leave a little more room for the board.
    auto drawPlayerPanel = [&](Player* player, sf::FloatRect rect, sf::Color accent)
    {
        ui->drawPanel(window, rect, accent);
        if (!player || player->getCharacters().empty()) return;

        Character* hero = player->getHero();
        ui->drawText(window, player->getName(), {rect.position.x + 18.f, rect.position.y + 14.f}, 15, accent);
        ui->drawText(window, hero->getName(), {rect.position.x + 18.f, rect.position.y + 40.f}, 13, PARCHMENT);

        const std::string hp = "HP  " + std::to_string(hero->getHp()) + "/" + std::to_string(hero->getMaxhp());
        ui->drawText(window, hp, {rect.position.x + 18.f, rect.position.y + 65.f}, 11, PARCHMENT);
        ui->drawHealth(window, {rect.position.x + 18.f, rect.position.y + 88.f},
                       static_cast<float>(hero->getHp()) / std::max(1, hero->getMaxhp()),
                       rect.size.x - 36.f, accent);

        if (player->getDeck())
        {
            ui->drawText(window, "DECK " + std::to_string(player->getDeck()->getdeckSize()),
                         {rect.position.x + 18.f, rect.position.y + 112.f}, 10, PARCHMENT);
            ui->drawText(window, "HAND " + std::to_string(player->getDeck()->gethandSize()),
                         {rect.position.x + 120.f, rect.position.y + 112.f}, 10, PARCHMENT);
            ui->drawText(window, "DISCARD " + std::to_string(player->getDeck()->getdiscardSize()),
                         {rect.position.x + 220.f, rect.position.y + 112.f}, 10, PARCHMENT);
        }

        // Hero portrait. If the real PNG is absent, the character token/initial
        // still provides a valid fallback.
        std::string id;
        if (hero->getName() == "Dracula") id = "dracula";
        else if (hero->getName() == "sherlock") id = "sherlock";
        else if (hero->getName() == "invisible man") id = "invisible_man";
        if (const sf::Texture* tex = textures.get(id))
        {
            sf::Sprite sprite(*tex);
            const sf::Vector2u size = tex->getSize();
            const float scale = std::min(64.f / static_cast<float>(size.x), 64.f / static_cast<float>(size.y));
            sprite.setScale({scale, scale});
            const sf::FloatRect b = sprite.getGlobalBounds();
            sprite.setPosition({rect.position.x + rect.size.x - b.size.x - 18.f,
                                 rect.position.y + 25.f});
            window.draw(sprite);
        }

        float y = rect.position.y + 145.f;
        for (int i = 1; i < player->getfighterCount(); ++i)
        {
            Character* c = player->getFighter(i);
            if (!c) continue;
            const std::string letter = c->getName() == "Dr_watson" ? "W" :
                (c->getName().find("Sister") != std::string::npos ? "S" : "I");
            ui->drawText(window, letter, {rect.position.x + 18.f, y}, 15, accent);
            ui->drawText(window, c->getName() + "  " + std::to_string(c->getHp()) + "/" + std::to_string(c->getMaxhp()),
                         {rect.position.x + 43.f, y + 2.f}, 9,
                         c->checkalive() ? PARCHMENT : sf::Color(100, 95, 90));
            y += 28.f;
        }
    };

    drawPlayerPanel(current, {{18.f, 85.f}, {385.f, 550.f}}, RED);
    drawPlayerPanel(enemy, {{1197.f, 85.f}, {385.f, 550.f}}, BLUE);

    std::vector<int> highlights;
    Character* selected = selectedCurrentCharacter();
    if (moveMode && selected)
        highlights = controller.getValidMoveSpaces(selected, selected->getMove());

    controller.getBord();
    boardView->draw(window, controller.getBord(), selectedSpace, highlights);

    // Exact logical space numbers and zone labels are rendered from Bord.
    for (int i = 0; i < 32; ++i)
    {
        const sf::Vector2f pos = boardView->getPosition(i);
        ui->drawText(window, std::to_string(i), {pos.x - 6.f, pos.y - 9.f}, 9, PARCHMENT);
        const std::vector<int> zones = controller.getBord().getposZone(i);
        if (!zones.empty())
        {
            std::string z = "Z";
            for (int zone : zones) z += std::to_string(zone) + (zone == zones.back() ? "" : "/");
            ui->drawText(window, z, {pos.x - 10.f, pos.y + 27.f}, 6,
                         zones.size() > 1 ? GOLD : sf::Color(190, 181, 158));
        }
    }

    // Fog tokens are stored in Invisible Man's actual Character state.
    auto drawFog = [&](Player* player)
    {
        if (!player || !player->getHero()) return;
        auto* im = dynamic_cast<invisible_man*>(player->getHero());
        if (!im) return;
        for (int pos : im->getMistTokens())
        {
            if (pos < 0 || pos >= 32) continue;
            sf::CircleShape fog(13.f);
            fog.setOrigin({13.f,13.f});
            fog.setPosition(boardView->getPosition(pos));
            fog.setFillColor(sf::Color(102, 103, 112, 95));
            fog.setOutlineColor(sf::Color(205, 197, 177, 170));
            fog.setOutlineThickness(1.f);
            window.draw(fog);
            ui->drawText(window, "F", {boardView->getPosition(pos).x - 4.f,
                                        boardView->getPosition(pos).y - 7.f}, 10, PARCHMENT);
        }
    };
    drawFog(current);
    drawFog(enemy);

    for (int i = 0; i < 32; ++i)
    {
        Character* c = controller.getCharacterAt(i);
        if (!c) continue;
        bool isSelected = selected == c || selectedEnemy == i;
        characterView->draw(window, c, boardView->getPosition(i), isSelected);
    }

    // Bottom action panel.
    ui->drawPanel(window, {{18.f, 650.f}, {385.f, 232.f}}, RED);
    ui->drawText(window, "ACTIONS", {35.f, 665.f}, 17, RED);
    ui->drawButton(window, {{35.f, 700.f}, {105.f, 43.f}}, "MOVE", moveMode, GREEN);
    ui->drawButton(window, {{150.f, 700.f}, {105.f, 43.f}}, "ATTACK", attackMode, RED);
    ui->drawButton(window, {{265.f, 700.f}, {105.f, 43.f}}, "SCHEME", false, PURPLE);
    ui->drawButton(window, {{35.f, 754.f}, {105.f, 43.f}}, "BOOST", boostMode, GOLD);
    ui->drawButton(window, {{150.f, 754.f}, {105.f, 43.f}}, "DRAW CARD", false, GREEN);
    ui->drawButton(window, {{265.f, 754.f}, {105.f, 43.f}}, "PLAY CARD", selectedCard >= 0, PURPLE);
    ui->drawButton(window, {{35.f, 808.f}, {335.f, 48.f}}, "END ACTION", false, GOLD);

    // Hand / card information.
    ui->drawPanel(window, {{420.f, 650.f}, {757.f, 232.f}}, GOLD);
    ui->drawText(window, "YOUR HAND", {440.f, 665.f}, 17, GOLD);
    if (current && current->getDeck())
    {
        cardView->drawHand(window, current->getDeck()->gethand(), selectedCard);
        if (selectedCard >= 0 && selectedCard < current->getDeck()->gethandSize())
        {
            const Card& card = current->getDeck()->gethand()[selectedCard];
            ui->drawText(window, card.getName(), {450.f, 688.f}, 9, PARCHMENT);
        }
    }

    // Right player/deck/turn panel.
    ui->drawPanel(window, {{1197.f, 650.f}, {385.f, 232.f}}, BLUE);
    ui->drawText(window, "TURN / DECK", {1215.f, 665.f}, 17, BLUE);
    if (current)
    {
        ui->drawText(window, current->getName(), {1215.f, 700.f}, 13, PARCHMENT);
        ui->drawText(window, "ACTION " + std::to_string(controller.getActionCount() + 1) + " / 2",
                     {1215.f, 725.f}, 10, sf::Color(170, 161, 147));
        if (current->getDeck())
        {
            deckView->draw(window, {1215.f, 750.f},
                           current->getDeck()->getdeckSize(),
                           current->getDeck()->gethandSize(),
                           current->getDeck()->getdiscardSize());
            ui->drawText(window, "DECK " + std::to_string(current->getDeck()->getdeckSize()),
                         {1300.f, 758.f}, 10, PARCHMENT);
            ui->drawText(window, "HAND " + std::to_string(current->getDeck()->gethandSize()),
                         {1300.f, 781.f}, 10, PARCHMENT);
            ui->drawText(window, "DISCARD " + std::to_string(current->getDeck()->getdiscardSize()),
                         {1300.f, 804.f}, 10, PARCHMENT);
        }
    }
    ui->drawButton(window, {{1385.f, 815.f}, {175.f, 42.f}}, "END TURN", true, BLUE);

    if (messageTimer > 0)
        ui->drawText(window, message, {430.f, 625.f}, 11, sf::Color(221, 184, 98));
}

void GameWindow::handleMainMenuClick(sf::Vector2f p)
{
    if (sf::FloatRect({575.f, 425.f}, {450.f, 60.f}).contains(p))
    {
        players[0].reset();
        players[1].reset();
        controller = Controller();

        player1Name = "PLAYER 1";
        player2Name = "PLAYER 2";
        player1Age.clear();
        player2Age.clear();
        activeInputField = 0;
        player2AI = true;

        setupStarted = controller.beginGuiSetup(players);
        screen = Screen::Setup;
        return;
    }

    if (sf::FloatRect({575.f, 585.f}, {450.f, 60.f}).contains(p))
        window.close();
}

void GameWindow::handleSetupClick(sf::Vector2f p)
{
    if (!setupStarted) return;

    const auto stage = controller.getGuiSetupStage();

    if (stage == Controller::GuiSetupStage::PlayerInfo)
    {
        if (sf::FloatRect({350.f, 225.f}, {380.f, 50.f}).contains(p))
            activeInputField = 0;
        else if (sf::FloatRect({350.f, 315.f}, {180.f, 50.f}).contains(p))
            activeInputField = 1;
        else if (!player2AI && sf::FloatRect({900.f, 225.f}, {380.f, 50.f}).contains(p))
            activeInputField = 2;
        else if (!player2AI && sf::FloatRect({900.f, 315.f}, {180.f, 50.f}).contains(p))
            activeInputField = 3;
        else if (sf::FloatRect({900.f, 410.f}, {380.f, 52.f}).contains(p))
        {
            player2AI = !player2AI;
            return;
        }
        else if (sf::FloatRect({520.f, 655.f}, {560.f, 62.f}).contains(p))
        {
            if (player1Age.empty() ||
                (!player2AI && player2Age.empty()))
            {
                showMessage("Enter a valid age for every human player.");
                return;
            }

            const int age1 = std::stoi(player1Age);
            const int age2 = player2AI ? age1 : std::stoi(player2Age);

            if (!controller.guiFinishPlayerSetup(
                    player1Name, age1,
                    player2AI ? "AI" : player2Name, age2, player2AI))
            {
                showMessage("Player information is invalid.");
                return;
            }
            showMessage("Player information accepted.");
        }
        return;
    }

    if (stage == Controller::GuiSetupStage::CharacterSelection)
    {
        const std::vector<int> choices = controller.getGuiCharacterChoices();
        for (int i = 0; i < 3; ++i)
        {
            if (sf::FloatRect({430.f, 225.f + i * 135.f}, {740.f, 105.f}).contains(p) &&
                std::find(choices.begin(), choices.end(), i + 1) != choices.end())
            {
                controller.guiChooseCharacter(i + 1);
                return;
            }
        }
        return;
    }

    if (stage == Controller::GuiSetupStage::HeroPosition)
    {
        if (sf::FloatRect({350.f, 270.f}, {390.f, 260.f}).contains(p))
        {
            controller.guiChooseHeroPosition(1);
            return;
        }
        if (sf::FloatRect({860.f, 270.f}, {390.f, 260.f}).contains(p))
        {
            controller.guiChooseHeroPosition(2);
            return;
        }
        return;
    }

    if (stage == Controller::GuiSetupStage::SidekickPlacement)
    {
        const int space = boardView->getSpaceAt(p);
        if (space >= 0)
        {
            const std::vector<int> valid = controller.getGuiPlacementSpaces();
            if (std::find(valid.begin(), valid.end(), space) != valid.end())
            {
                controller.guiPlaceSidekick(space);
                return;
            }
        }
        return;
    }

    if (stage == Controller::GuiSetupStage::Ready)
    {
        if (sf::FloatRect({520.f, 470.f}, {560.f, 62.f}).contains(p))
        {
            screen = Screen::Game;
            resetSelections();
            showMessage("The game has started.");
        }
    }
}

void GameWindow::handleGameClick(sf::Vector2f p)
{
    Player* current = controller.getCurrentPlayer();
    Character* selected = selectedCurrentCharacter();
    if (sf::FloatRect({1472.f, 12.f}, {105.f, 42.f}).contains(p))
    {
        window.close();
        return;
    }

    if (sf::FloatRect({1385.f, 815.f}, {175.f, 42.f}).contains(p))
    {
        controller.guiEndTurn();
        resetSelections();
        showMessage("Turn changed.");
        return;
    }

    if (sf::FloatRect({35.f, 700.f}, {105.f, 43.f}).contains(p))
    {
        moveMode = true;
        attackMode = false;
        boostMode = false;
        showMessage("Select one of your fighters, then a highlighted destination.");
        return;
    }

    if (sf::FloatRect({150.f, 700.f}, {105.f, 43.f}).contains(p))
    {
        attackMode = true;
        moveMode = false;
        boostMode = false;
        showMessage("Select an attack/versatile card, then an enemy target.");
        return;
    }

    if (sf::FloatRect({265.f, 700.f}, {105.f, 43.f}).contains(p))
    {
        moveMode = false;
        attackMode = false;
        boostMode = false;
        showMessage("Scheme cards are selected from your hand.");
        return;
    }

    if (sf::FloatRect({35.f, 754.f}, {105.f, 43.f}).contains(p))
    {
        boostMode = true;
        moveMode = false;
        attackMode = false;
        showMessage("Select a card with the required boost value.");
        return;
    }

    if (sf::FloatRect({150.f, 754.f}, {105.f, 43.f}).contains(p))
    {
        if (controller.guiDrawCard())
        {
            controller.guiEndAction();
            showMessage("1 card drawn from the real deck.");
        }
        else showMessage("The deck is empty; the existing rule was applied.");
        return;
    }

    if (sf::FloatRect({265.f, 754.f}, {105.f, 43.f}).contains(p))
    {
        if (selectedCard >= 0 && controller.guiPlayCard(selectedCard))
        {
            controller.guiEndAction();
            selectedCard = -1;
            showMessage("Card removed through the real Deck::playCard().");
        }
        else showMessage("Select a card first.");
        return;
    }

    if (sf::FloatRect({35.f, 808.f}, {335.f, 48.f}).contains(p))
    {
        controller.guiEndAction();
        resetSelections();
        showMessage("Action ended.");
        return;
    }

    if (current && current->getDeck())
    {
        const int card = cardView->getCardAt(p, current->getDeck()->gethandSize());
        if (card >= 0)
        {
            selectedCard = card;
            if (boostMode) showMessage("Boost " + std::to_string(current->getDeck()->getHandcard(card).getBoost()) + " selected.");
            return;
        }
    }

    const int space = boardView->getSpaceAt(p);
    if (space < 0) return;
    selectedSpace = space;

    Character* occupant = controller.getCharacterAt(space);
    if (occupant && controller.isCurrentPlayer(occupant))
    {
        selectedEnemy = -1;
        for (int i = 0; current && i < current->getfighterCount(); ++i)
            if (current->getFighter(i) == occupant) selectedCharacter = i;
        showMessage(occupant->getName() + " selected.");
        return;
    }

    if (occupant && !controller.isCurrentPlayer(occupant))
    {
        selectedEnemy = space;
        if (attackMode && selected && selectedCard >= 0 && current->getDeck())
        {
            Player* enemy = controller.getEnemyPlayer();
            int defense = -1;
            if (enemy && enemy->getDeck())
            {
                for (int i = 0; i < enemy->getDeck()->gethandSize(); ++i)
                {
                    Card c = enemy->getDeck()->getHandcard(i);
                    if (c.isDefense() || c.isVersatile()) { defense = i; break; }
                }
            }
            if (defense >= 0 && controller.guiAttack(selected, occupant, selectedCard, defense))
            {
                resetSelections();
                controller.guiEndAction();
                showMessage("Combat resolved through the existing Controller rules.");
            }
            else showMessage("That target/card combination is not legal.");
        }
        return;
    }

    if (moveMode && selected)
    {
        if (controller.guiMove(selected, selected->getMove(), space))
        {
            controller.guiEndAction();
            selectedSpace = space;
            showMessage("Character moved using the real Bord adjacency.");
        }
        else showMessage("That destination is not legal for this character.");
    }
}

void GameWindow::startGame()
{
    players[0].reset();
    players[1].reset();
    controller = Controller();

    if (!controller.startGuiGame(players, selectedHero1, selectedHero2,
                                  "PLAYER 1", "PLAYER 2", player2AI))
    {
        showMessage("Could not initialize the game.");
        return;
    }

    screen = Screen::Game;
    resetSelections();
}

void GameWindow::resetSelections()
{
    selectedSpace = -1;
    selectedCharacter = -1;
    selectedCard = -1;
    selectedEnemy = -1;
    attackMode = false;
    moveMode = false;
    boostMode = false;
}

Character* GameWindow::selectedCurrentCharacter() const
{
    Player* current = controller.getCurrentPlayer();
    if (!current || selectedCharacter < 0 || selectedCharacter >= current->getfighterCount())
        return nullptr;
    return current->getFighter(selectedCharacter);
}
