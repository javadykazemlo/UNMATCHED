#include "graphics/GameWindow.hpp"
#include "graphics/BoardView.hpp"
#include "graphics/CharacterView.hpp"
#include "graphics/CardView.hpp"
#include "graphics/DeckView.hpp"
#include "graphics/UI.hpp"
#include "graphics/RulesView.hpp"
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
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <unordered_set>
#include <nlohmann/json.hpp>

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
    rulesView = std::make_unique<RulesView>(font);
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

        if (screen == Screen::Game && rulesView && rulesView->isOpen())
        {
            sf::Vector2f mousePosition{0.f, 0.f};
            if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
                mousePosition = window.mapPixelToCoords(mouse->position);
            else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
                mousePosition = window.mapPixelToCoords(mouse->position);

            rulesView->handleEvent(*event, mousePosition);
            continue;
        }

        // Graphical input box (typed number) for whichever effect panel is
        // currently active. Each panel keeps its own input buffer; this is
        // just keystroke routing to the right one, not panel logic.
        if (screen == Screen::Game && activeEffectPanel == EffectPanelKind::Attack && attackInteger)
        {
            if (const auto* text = event->getIf<sf::Event::TextEntered>())
            {
                if (text->unicode >= '0' && text->unicode <= '9' &&
                    attackInputBuffer.size() < 3)
                    attackInputBuffer.push_back(static_cast<char>(text->unicode));
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Backspace && !attackInputBuffer.empty())
                    attackInputBuffer.pop_back();
                else if (key->code == sf::Keyboard::Key::Enter && !attackInputBuffer.empty())
                {
                    if (controller.submitGuiInput(std::stoi(attackInputBuffer)))
                        attackInputBuffer.clear();
                }
            }
        }
        else if (screen == Screen::Game && activeEffectPanel == EffectPanelKind::Scheme && schemeInteger)
        {
            if (const auto* text = event->getIf<sf::Event::TextEntered>())
            {
                if (text->unicode >= '0' && text->unicode <= '9' &&
                    schemeInputBuffer.size() < 3)
                    schemeInputBuffer.push_back(static_cast<char>(text->unicode));
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Backspace && !schemeInputBuffer.empty())
                    schemeInputBuffer.pop_back();
                else if (key->code == sf::Keyboard::Key::Enter && !schemeInputBuffer.empty())
                {
                    if (controller.submitGuiInput(std::stoi(schemeInputBuffer)))
                        schemeInputBuffer.clear();
                }
            }
        }
        else if (screen == Screen::Game && activeEffectPanel == EffectPanelKind::Dracula && draculaInteger)
        {
            if (const auto* text = event->getIf<sf::Event::TextEntered>())
            {
                if (text->unicode >= '0' && text->unicode <= '9' &&
                    draculaInputBuffer.size() < 3)
                    draculaInputBuffer.push_back(static_cast<char>(text->unicode));
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Backspace && !draculaInputBuffer.empty())
                    draculaInputBuffer.pop_back();
                else if (key->code == sf::Keyboard::Key::Enter && !draculaInputBuffer.empty())
                {
                    if (controller.submitGuiInput(std::stoi(draculaInputBuffer)))
                        draculaInputBuffer.clear();
                }
            }
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (screen == Screen::LoadGame && key->code == sf::Keyboard::Key::Escape)
            {
                screen = Screen::MainMenu;
                loadScroll = 0.f;
                continue;
            }
        }

        if (screen == Screen::LoadGame)
        {
            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                loadScroll -= wheel->delta * 55.f;
                const float visible = 610.f;
                const float rowHeight = 86.f;
                const float contentHeight = static_cast<float>(saveEntries.size()) * rowHeight;
                const float maxScroll = std::max(0.f, contentHeight - visible);
                loadScroll = std::clamp(loadScroll, 0.f, maxScroll);
                continue;
            }
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

            if (screen == Screen::Game && rulesView && rulesView->isOpen())
            {
                rulesView->handleEvent(*event, p);
                continue;
            }

            if (screen == Screen::MainMenu) handleMainMenuClick(p);
            else if (screen == Screen::LoadGame) handleLoadGameClick(p);
            else if (screen == Screen::Setup) handleSetupClick(p);
            else if (screen == Screen::Game) handleGameClick(p);
            else if (screen == Screen::GameOver)
            {
                if (sf::FloatRect({610.f, 545.f}, {380.f, 50.f}).contains(p))
                    screen = Screen::MainMenu;
            }
        }
    }
}

void GameWindow::update()
{
    if (messageTimer > 0) --messageTimer;

    if (screen == Screen::Game)
    {
        switch (activeEffectPanel)
        {
            case EffectPanelKind::Attack:  updateAttackEffectPanel();  break;
            case EffectPanelKind::Scheme:  updateSchemeEffectPanel();  break;
            case EffectPanelKind::Dracula: updateDraculaEffectPanel(); break;
            case EffectPanelKind::None:    break;
        }

        if (!controller.guiEffectBusy() && activeEffectPanel == EffectPanelKind::None)
            checkGameOver();
    }
}

void GameWindow::render()
{
    if (screen == Screen::MainMenu) drawMainMenu();
    else if (screen == Screen::LoadGame) drawLoadGame();
    else if (screen == Screen::Setup) drawSetup();
    else if (screen == Screen::GameOver) drawGameOver();
    else drawGame();

    if (screen == Screen::Game && rulesView && rulesView->isOpen())
        rulesView->draw(window, {1600.f, 900.f});
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

    ui->drawText(window, "created & Developed by", {700.f, 700.f}, 14, sf::Color(170, 162, 150));
    ui->drawText(window, "Mahdi Dehnavi & Mohammd Javad Kazemlo", {640.f, 722.f}, 14, sf::Color(170, 162, 150));
}

void GameWindow::refreshSaveEntries()
{
    namespace fs = std::filesystem;
    using json = nlohmann::json;

    saveEntries.clear();
    std::unordered_set<std::string> seen;
    std::vector<fs::path> roots;
    std::error_code ec;

    fs::path currentSaveDir = fs::absolute("saves", ec);
    if (!ec) roots.push_back(currentSaveDir);

    fs::path parentSaveDir = fs::absolute(fs::path("..") / "saves", ec);
    if (!ec && parentSaveDir != currentSaveDir) roots.push_back(parentSaveDir);

    for (const fs::path& dir : roots)
    {
        if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec))
            continue;

        for (const auto& entry : fs::directory_iterator(dir, ec))
        {
            if (ec) break;
            if (!entry.is_regular_file(ec) || entry.path().extension() != ".json")
                continue;

            const std::string canonical = fs::weakly_canonical(entry.path(), ec).string();
            if (!seen.insert(canonical).second)
                continue;

            try
            {
                std::ifstream file(entry.path());
                if (!file.is_open()) continue;
                json root;
                file >> root;
                const json& state = root.contains("gameState") ? root.at("gameState") : root;
                if (!state.contains("players") || state["players"].size() < 2)
                    continue;

                SaveEntry info;
                info.path = entry.path().string();
                info.timestamp = 0;

                if (root.contains("saveMetadata"))
                {
                    const auto& meta = root.at("saveMetadata");
                    info.date = meta.value("date", "");
                    info.time = meta.value("time", "");
                    info.timestamp = meta.value("timestamp", 0LL);
                }

                if (info.timestamp == 0)
                {
                    const auto last = fs::last_write_time(entry.path(), ec);
                    if (!ec)
                    {
                        const auto sys = std::chrono::time_point_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now() +
                            (last - fs::file_time_type::clock::now()));
                        info.timestamp = sys.time_since_epoch().count();
                    }
                }

                if (info.date.empty() || info.time.empty())
                {
                    const std::time_t tt = std::chrono::system_clock::to_time_t(
                        std::chrono::system_clock::time_point(std::chrono::milliseconds(info.timestamp)));
                    std::tm localTm{};
#ifdef _WIN32
                    localtime_s(&localTm, &tt);
#else
                    localtime_r(&tt, &localTm);
#endif
                    std::ostringstream date, time;
                    date << std::put_time(&localTm, "%Y-%m-%d");
                    time << std::put_time(&localTm, "%H:%M:%S");
                    info.date = date.str();
                    info.time = time.str();
                }

                const auto& p0 = state["players"][0];
                const auto& p1 = state["players"][1];
                info.player1 = p0.value("name", "PLAYER 1");
                info.player2 = p1.value("name", "PLAYER 2");

                auto heroName = [](const json& p) -> std::string
                {
                    const int choice = p.value("heroChoice", 0);
                    if (choice == 1) return "Dracula";
                    if (choice == 2) return "Sherlock";
                    if (choice == 3) return "Invisible Man";
                    return "Unknown Hero";
                };
                info.heroes = heroName(p0) + " vs " + heroName(p1);
                saveEntries.push_back(std::move(info));
            }
            catch (...) { /* ignore one bad save; keep the others */ }
        }
    }

    std::sort(saveEntries.begin(), saveEntries.end(),
              [](const SaveEntry& a, const SaveEntry& b)
              {
                  if (a.timestamp != b.timestamp) return a.timestamp > b.timestamp;
                  return a.path > b.path;
              });
}

void GameWindow::drawLoadGame()
{
    drawFullscreenTexture("main_menu");

    sf::RectangleShape overlay({1600.f, 900.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 155));
    window.draw(overlay);

    sf::RectangleShape panel({1240.f, 660.f});
    panel.setPosition({180.f, 110.f});
    panel.setFillColor(sf::Color(10, 11, 17, 245));
    panel.setOutlineColor(GOLD);
    panel.setOutlineThickness(2.f);
    window.draw(panel);

    ui->drawText(window, "LOAD GAME", {720.f, 140.f}, 32, GOLD);
    ui->drawText(window, "SELECT A SAVED BATTLE TO CONTINUE FROM THE EXACT SAVED STATE.",
                 {555.f, 185.f}, 10, PARCHMENT);

    sf::RectangleShape listPanel({1150.f, 450.f});
    listPanel.setPosition({225.f, 230.f});
    listPanel.setFillColor(sf::Color(6, 7, 12, 210));
    listPanel.setOutlineColor(sf::Color(95, 82, 60));
    listPanel.setOutlineThickness(1.f);
    window.draw(listPanel);

    if (saveEntries.empty())
    {
        ui->drawText(window, "NO SAVED GAMES FOUND", {650.f, 465.f}, 16, PARCHMENT);
        ui->drawText(window, "USE SAVE GAME DURING A BATTLE TO CREATE A SAVE.",
                     {580.f, 500.f}, 10, sf::Color(145, 138, 126));
    }
    else
    {
        const float rowHeight = 86.f;
        for (std::size_t i = 0; i < saveEntries.size(); ++i)
        {
            const SaveEntry& save = saveEntries[i];
            const float y = 230.f + 12.f + static_cast<float>(i) * rowHeight - loadScroll;
            if (y + 74.f < 230.f || y > 680.f) continue;

            sf::RectangleShape row({1110.f, 74.f});
            row.setPosition({245.f, y});
            row.setFillColor(sf::Color(15, 16, 23, 245));
            row.setOutlineColor(sf::Color(74, 65, 50));
            row.setOutlineThickness(1.f);
            window.draw(row);

            ui->drawText(window, save.date + "   " + save.time, {265.f, y + 9.f}, 12, GOLD);
            ui->drawText(window, save.heroes, {265.f, y + 32.f}, 13, PARCHMENT);
            ui->drawText(window, save.player1 + "  vs  " + save.player2,
                         {265.f, y + 54.f}, 9, sf::Color(155, 149, 138));
            ui->drawText(window, "LOAD", {1210.f, y + 26.f}, 11, GOLD);
        }
    }

    ui->drawButton(window, {{745.f, 755.f}, {175.f, 45.f}}, "BACK", false, GOLD);
    ui->drawText(window, "ESC: BACK  |  MOUSE WHEEL: SCROLL", {690.f, 810.f}, 9,
                 sf::Color(145, 138, 126));
}

void GameWindow::handleLoadGameClick(sf::Vector2f p)
{
    if (sf::FloatRect({745.f, 755.f}, {175.f, 45.f}).contains(p))
    {
        screen = Screen::MainMenu;
        loadScroll = 0.f;
        return;
    }

    if (saveEntries.empty()) return;

    const float top = 230.f;
    const float bottom = 680.f;
    if (p.y < top || p.y > bottom) return;

    const float rowHeight = 86.f;
    const int index = static_cast<int>((p.y - top + loadScroll - 12.f) / rowHeight);
    if (index < 0 || index >= static_cast<int>(saveEntries.size())) return;

    const float rowTop = top + 12.f + index * rowHeight - loadScroll;
    if (p.y < rowTop || p.y > rowTop + 74.f) return;

    // Replace the temporary menu/setup state with the selected save state.
    players[0].reset();
    players[1].reset();
    controller = Controller();

    if (!controller.LoadGame(players, saveEntries[index].path))
    {
        showMessage("Could not load the selected saved game.");
        return;
    }

    screen = Screen::Game;
    resetSelections();
    combatLog.clear();
    setupStarted = false;
    message.clear();
    messageTimer = 0;
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
                 {710.f, 17.f}, 18, turnColor);
    ui->drawText(window, "HERO PHASE", {760.f, 43.f}, 10, sf::Color(150, 143, 132));
    ui->drawButton(window, {{1375.f, 12.f}, {90.f, 42.f}}, "RULES",
                   rulesView && rulesView->isOpen(), GOLD);
    ui->drawButton(window, {{1472.f, 12.f}, {105.f, 42.f}}, "EXIT", false, GOLD);

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
    if (moveMode && selected && !awaitingMoveBoost)
        highlights = controller.getValidMoveSpaces(selected, selected->getMove() + moveBoost);

    controller.getBord();
    boardView->draw(window, controller.getBord(), selectedSpace, highlights);

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

    ui->drawPanel(window, {{18.f, 650.f}, {385.f, 232.f}}, RED);
    ui->drawText(window, "ACTIONS", {35.f, 665.f}, 17, RED);
    const bool canAct = controller.getActionCount() < 2;
    ui->drawButton(window, {{35.f, 700.f}, {78.f, 43.f}}, "MOVE", moveMode, GREEN);
    ui->drawButton(window, {{120.f, 700.f}, {78.f, 43.f}}, "ATTACK", attackMode, RED);
    ui->drawButton(window, {{205.f, 700.f}, {78.f, 43.f}}, "SCHEME", schemeMode, PURPLE);
    bool abilityAvailable = controller.guiHeroAbilityAvailable();
    if (current && current->getHero())
    {
        const std::string heroName = current->getHero()->getName();
        abilityAvailable = abilityAvailable || heroName == "sherlock" || heroName == "invisible man";
    }
    ui->drawButton(window, {{290.f, 700.f}, {78.f, 43.f}}, "ABILITY", false,
                   abilityAvailable ? GOLD : sf::Color(70, 65, 55));

    ui->drawButton(window, {{35.f, 754.f}, {160.f, 43.f}}, "SAVE GAME", false, GOLD);
    ui->drawButton(window, {{210.f, 754.f}, {160.f, 43.f}}, "END ACTION", false, GOLD);

    if (moveBoostPrompt)
    {
        ui->drawText(window, "USE BOOST?", {35.f, 831.f}, 11, GOLD);
        ui->drawButton(window, {{125.f, 820.f}, {90.f, 36.f}}, "YES", true, GREEN);
        ui->drawButton(window, {{225.f, 820.f}, {90.f, 36.f}}, "NO", false, GOLD);
    }
    else
    {
        ui->drawText(window, std::to_string(controller.getActionCount()) + " / 2 ACTIONS",
                     {105.f, 812.f}, 12, PARCHMENT);
    }

    // Message / combat report panel below the board.
    ui->drawPanel(window, {{420.f, 606.f}, {757.f, 66.f}}, combatLog.empty() ? GOLD : RED);
    if (!combatLog.empty())
    {
        const int first = std::max(0, static_cast<int>(combatLog.size()) - 3);
        float y = 612.f;
        for (int i = first; i < static_cast<int>(combatLog.size()); ++i)
        {
            std::string line = combatLog[i];
            if (line.size() > 105) line = line.substr(0, 102) + "...";
            ui->drawText(window, line, {435.f, y}, 11, PARCHMENT);
            y += 18.f;
        }
    }
    else if (messageTimer > 0)
    {
        ui->drawText(window, message, {435.f, 621.f}, 16, GOLD);
    }

    // Hand / card information.
    ui->drawPanel(window, {{420.f, 680.f}, {757.f, 215.f}}, defenseSelectionMode ? BLUE : GOLD);
    if (defenseSelectionMode && enemy && enemy->getDeck())
    {
        ui->drawText(window, enemy->getName() + " - CHOOSE DEFENSE", {440.f, 688.f}, 17, BLUE);
        cardView->drawHand(window, enemy->getDeck()->gethand(), selectedCard);
        ui->drawText(window, "Select a defense / versatile card for the defending fighter.",
                     {440.f, 863.f}, 9, PARCHMENT);
    }
    else
    {
        ui->drawText(window, handLimitMode ? "DISCARD DOWN TO 7" : "YOUR HAND",
                     {440.f, 688.f}, 17, handLimitMode ? RED : GOLD);
        if (current && current->getDeck())
        {
            cardView->drawHand(window, current->getDeck()->gethand(), selectedCard);
            if (selectedCard >= 0 && selectedCard < current->getDeck()->gethandSize())
            {
                const Card& card = current->getDeck()->getHandcard(selectedCard);
                ui->drawText(window, card.getName(), {450.f, 710.f}, 9, PARCHMENT);
            }
        }
        if (handLimitMode)
            ui->drawText(window, "Choose a card to discard. Your turn will continue when the hand reaches 7.",
                         {440.f, 863.f}, 9, RED);
        else if (awaitingMoveBoost)
            ui->drawText(window, "Optional BOOST: choose a card, or select a fighter to skip it.",
                         {440.f, 863.f}, 9, GOLD);
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

    switch (activeEffectPanel)
    {
        case EffectPanelKind::Attack:  drawAttackEffectPanel();  break;
        case EffectPanelKind::Scheme:  drawSchemeEffectPanel();  break;
        case EffectPanelKind::Dracula: drawDraculaEffectPanel(); break;
        case EffectPanelKind::None:    break;
    }
}

void GameWindow::drawGameOver()
{
    drawFullscreenTexture("game");
    sf::RectangleShape overlay({1600.f, 900.f});
    overlay.setFillColor(sf::Color(3, 4, 8, 220));
    window.draw(overlay);

    ui->drawText(window, "THE BATTLE IS OVER", {570.f, 170.f}, 38, GOLD);
    ui->drawPanel(window, {{390.f, 280.f}, {820.f, 350.f}}, GOLD);
    ui->drawText(window, "CONGRATULATIONS", {630.f, 330.f}, 26, PARCHMENT);
    ui->drawText(window, winnerName + "", {700.f, 385.f}, 28, GOLD);
    ui->drawText(window, "has won the battle.", {675.f, 430.f}, 16, PARCHMENT);
    ui->drawText(window, "Thank you for playing UNMATCHED.", {637.f, 475.f}, 14, sf::Color(170, 162, 150));
    ui->drawText(window, "Farewell, and until the next battle...", {637.f, 505.f}, 14, sf::Color(170, 162, 150));
    ui->drawButton(window, {{610.f, 545.f}, {380.f, 50.f}}, "RETURN TO MAIN MENU", true, GOLD);

    ui->drawText(window, "created & Developed by", {700.f, 700.f}, 14, sf::Color(170, 162, 150));
    ui->drawText(window, "Mahdi Dehnavi & Mohammd Javad Kazemlo", {640.f, 722.f}, 14, sf::Color(170, 162, 150));

}

void GameWindow::checkGameOver()
{
    if (!controller.isGameOver()) return;
    winnerName = controller.getGuiWinnerName();
    combatLog = controller.getGuiCombatLog();
    resetSelections();
    screen = Screen::GameOver;
}

void GameWindow::refreshCombatLog()
{
    combatLog = controller.getGuiCombatLog();
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

    if (sf::FloatRect({575.f, 505.f}, {450.f, 60.f}).contains(p))
    {
        refreshSaveEntries();
        loadScroll = 0.f;
        screen = Screen::LoadGame;
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
            controller.guiBeginTurn();
            showMessage("The game has started.");
        }
    }
}

void GameWindow::handleGameClick(sf::Vector2f p)
{
    Player* current = controller.getCurrentPlayer();
    Player* enemy = controller.getEnemyPlayer();
    Character* selected = selectedCurrentCharacter();

    // Keep the game locked while a card effect is resolving or while the
    // resolved-effect result is still being displayed. Each panel routes
    // clicks through its own independent handler.
    if (activeEffectPanel != EffectPanelKind::None)
    {
        // The effect panel owns mouse input for its entire lifetime. The
        // individual handler decides whether it is waiting for an answer or
        // is ready for RETURN TO GAME. Do not gate this on busy: busy becomes
        // false as soon as the worker finishes, while the result panel stays
        // visible until the player closes it.
        switch (activeEffectPanel)
        {
            case EffectPanelKind::Attack:  handleAttackEffectInput(p);  break;
            case EffectPanelKind::Scheme:  handleSchemeEffectInput(p);  break;
            case EffectPanelKind::Dracula: handleDraculaEffectInput(p); break;
            case EffectPanelKind::None:    break;
        }
        return;
    }

    if (sf::FloatRect({1375.f, 12.f}, {90.f, 42.f}).contains(p))
    {
        if (rulesView)
            rulesView->open();
        return;
    }

    if (sf::FloatRect({1472.f, 12.f}, {105.f, 42.f}).contains(p))
    {
        window.close();
        return;
    }

    // Note: activeEffectPanel is always set together with the Controller's
    // busy flag at every call site below, so controller.guiEffectBusy()
    // cannot be true here (activeEffectPanel == None already handled above).

    // Hand limit: exactly the same rule as Controller::playTurn.
    // If the hand is above 7, the player must choose cards to discard first.
    if (handLimitMode)
    {
        if (current && current->getDeck())
        {
            const int card = cardView->getCardAt(p, current->getDeck()->gethandSize());
            if (card >= 0 && controller.guiDiscardCard(card))
            {
                if (current->getDeck()->gethandSize() <= 7)
                    finishTurnAfterHandLimit();
                else
                    showMessage("Discard another card. Hand must be 7 or fewer.");
            }
        }
        return;
    }

    // End Turn keeps its original position, but the hand limit is resolved
    // before the turn is actually passed to the opponent.
    if (sf::FloatRect({1385.f, 815.f}, {175.f, 42.f}).contains(p))
    {
        if (current && current->getDeck() && current->getDeck()->gethandSize() > 7)
        {
            handLimitMode = true;
            showMessage("You have more than 7 cards. Select cards to discard.");
            return;
        }

        controller.guiEndTurn();
        resetSelections();
        combatLog.clear();
        controller.guiBeginTurn();
        showMessage("Turn changed. Choose MOVE, ATTACK, SCHEME, or ABILITY.");
        return;
    }

    // SAVE GAME: every click creates a new file in a persistent saves folder.
    if (sf::FloatRect({35.f, 754.f}, {160.f, 43.f}).contains(p))
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        fs::path saveDir = fs::absolute("saves", ec);
        if (ec || !fs::exists(saveDir, ec))
            fs::create_directories(saveDir, ec);

        if (ec)
        {
            showMessage("Could not create the saves directory.");
            return;
        }

        const auto now = std::chrono::system_clock::now();
        const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count() % 1000;
        const std::time_t tt = std::chrono::system_clock::to_time_t(now);
        std::tm localTm{};
#ifdef _WIN32
        localtime_s(&localTm, &tt);
#else
        localtime_r(&tt, &localTm);
#endif
        std::ostringstream stamp;
        stamp << std::put_time(&localTm, "%Y-%m-%d_%H-%M-%S")
              << '-' << std::setw(3) << std::setfill('0') << millis;

        fs::path savePath = saveDir / ("save_" + stamp.str() + ".json");
        int suffix = 1;
        while (fs::exists(savePath, ec))
            savePath = saveDir / ("save_" + stamp.str() + "_" + std::to_string(suffix++) + ".json");

        if (!controller.guiSaveGame(savePath.string()))
        {
            showMessage("Could not save the game.");
            return;
        }

        combatLog.clear();
        showMessage("Game saved successfully.");
        return;
    }

    // END ACTION: immediately cancels the currently selected action/mode.
    if (sf::FloatRect({210.f, 754.f}, {160.f, 43.f}).contains(p))
    {
        resetSelections();
        combatLog.clear();
        showMessage("Current action ended.");
        return;
    }

    if (moveBoostPrompt)
    {
        if (sf::FloatRect({125.f, 820.f}, {90.f, 36.f}).contains(p))
        {
            moveBoostPrompt = false;
            awaitingMoveBoost = false;
            boostMode = true;
            showMessage("Boost confirmed: choose a boost card from your hand.");
            return;
        }

        if (sf::FloatRect({225.f, 820.f}, {90.f, 36.f}).contains(p))
        {
            moveBoostPrompt = false;
            awaitingMoveBoost = false;
            boostMode = false;
            moveBoost = 0;
            showMessage("No boost used. Choose a fighter to move.");
            return;
        }

        return;
    }

    const bool canAct = controller.getActionCount() < 2;

    // HERO ABILITY: Dracula has an active ability. Sherlock and Invisible Man
    // have passive abilities, so the same button opens a readable ability
    // panel without consuming an action or asking for input.
    if (sf::FloatRect({290.f, 700.f}, {78.f, 43.f}).contains(p))
    {
        if (!current || !current->getHero())
            return;

        const std::string heroName = current->getHero()->getName();

        if (heroName == "Dracula")
        {
            if (!controller.guiHeroAbilityAvailable())
            {
                showMessage("Dracula's ability is not available right now.");
                return;
            }

            resetSelections();
            combatLog.clear();
            if (controller.guiUseHeroAbility())
            {
                activeEffectPanel = EffectPanelKind::Dracula;
                draculaResolved = false;
                draculaPrompt = "Use Dracula's ability?";
                draculaChoices.clear();
                draculaYesNo = true;
                draculaInteger = false;
                draculaInputBuffer.clear();
                showMessage("Dracula ability opened. Choose YES or NO.");
            }
        }
        else if (heroName == "sherlock" || heroName == "invisible man")
        {
            resetSelections();
            combatLog.clear();
            activeEffectPanel = EffectPanelKind::Dracula;
            draculaResolved = true;
            draculaChoices.clear();
            draculaYesNo = false;
            draculaInteger = false;
            draculaInputBuffer.clear();
            if (heroName == "sherlock")
                draculaPrompt = "PASSIVE ABILITY — Sherlock: effects that attempt to disable Sherlock/Dr. Watson-related cards do not disable them.";
            else
                draculaPrompt = "PASSIVE ABILITY — Invisible Man: +1 defense on a fog space (not a card effect) and movement between fog spaces is treated as adjacency.";
        }
        else
        {
            showMessage("This fighter has no special ability.");
        }
        return;
    }

    // Each real action is counted immediately when its button is pressed.
    if (sf::FloatRect({35.f, 700.f}, {78.f, 43.f}).contains(p))
    {
        if (!canAct) { showMessage("You have already used both actions."); return; }
        controller.guiEndAction();
        moveMode = true;
        attackMode = false;
        schemeMode = false;
        defenseSelectionMode = false;
        selectedCharacter = -1;
        selectedCard = -1;
        selectedEnemy = -1;
        moveBoost = 0;
        awaitingMoveBoost = true;
        moveBoostPrompt = true;
        combatLog.clear();

        if (controller.guiDrawCard())
            showMessage("Maneuver: card drawn. Use a boost or skip it.");
        else
            showMessage("Maneuver started. The deck is empty; fatigue was applied.");
        return;
    }

    if (sf::FloatRect({120.f, 700.f}, {78.f, 43.f}).contains(p))
    {
        if (!canAct) { showMessage("You have already used both actions."); return; }
        controller.guiEndAction();
        moveMode = false;
        attackMode = true;
        schemeMode = false;
        defenseSelectionMode = false;
        awaitingMoveBoost = false;
        selectedCharacter = -1;
        selectedCard = -1;
        selectedEnemy = -1;
        combatLog.clear();
        controller.clearGuiCombatLog();
        showMessage("Attack: select a fighter, then an attack card and an enemy target.");
        return;
    }

    if (sf::FloatRect({205.f, 700.f}, {78.f, 43.f}).contains(p))
    {
        if (!canAct) { showMessage("You have already used both actions."); return; }
        controller.guiEndAction();
        moveMode = false;
        attackMode = false;
        schemeMode = true;
        defenseSelectionMode = false;
        awaitingMoveBoost = false;
        selectedCharacter = -1;
        selectedCard = -1;
        selectedEnemy = -1;
        combatLog.clear();
        showMessage("Scheme: select a fighter, then a Scheme card.");
        return;
    }

    // During attack resolution the opponent chooses from their own hand.
    if (defenseSelectionMode && enemy)
    {
        if (!enemy->getDeck()) return;
        const int defenseCard = cardView->getCardAt(p, enemy->getDeck()->gethandSize());
        if (defenseCard < 0) return;
        Character* defender = controller.getCharacterAt(selectedEnemy);
        const std::vector<int> validDefense = controller.getGuiDefenseCards(defender);
        if (std::find(validDefense.begin(), validDefense.end(), defenseCard) == validDefense.end())
        {
            showMessage("Choose a defense or versatile card allowed for this fighter.");
            return;
        }

        Character* attacker = selectedCurrentCharacter();
        if (controller.guiAttack(attacker, defender, selectedCard, defenseCard))
        {
            const Card attackCard = current->getDeck()->getHandcard(selectedCard);
            const Card defensePlayedCard = enemy->getDeck()->gethand()[defenseCard];
            activeEffectPanel = EffectPanelKind::Attack;
            attackResolved = false;
            attackCardName = "COMBAT";
            attackCardText = attackCard.getName() + "  VS  " + defensePlayedCard.getName() +
                             "\n\nATTACK EFFECT: " + (attackCard.geteffect().empty() ? "None" : attackCard.geteffect()) +
                             "\nDEFENSE EFFECT: " + (defensePlayedCard.geteffect().empty() ? "None" : defensePlayedCard.geteffect()) +
                             "\n\nResolving attack, defense and combat effects...";
            attackPrompt = "Resolving combat...";
            attackChoices.clear();
            attackYesNo = false;
            attackInteger = false;
            attackInputBuffer.clear();
            refreshCombatLog();
            defenseSelectionMode = false;
            attackMode = false;
            selectedCard = -1;
            selectedEnemy = -1;
            selectedCharacter = -1;
            showMessage("Attack resolved.");
            checkGameOver();
        }
        else showMessage("That defense card is not legal for this defender.");
        return;
    }

    // Card click belongs to the current player's hand.
    if (current && current->getDeck())
    {
        const int card = cardView->getCardAt(p, current->getDeck()->gethandSize());
        if (card >= 0)
        {
            if (moveMode && boostMode)
            {
                int boost = 0;
                if (controller.guiUseBoostCard(card, boost))
                {
                    moveBoost = boost;
                    boostMode = false;
                    selectedCard = -1;
                    showMessage("Boost used: +" + std::to_string(boost) + " movement. Now choose a fighter.");
                }
                return;
            }

            if (attackMode && selected)
            {
                const std::vector<int> valid = controller.getGuiAttackCards(selected);
                if (std::find(valid.begin(), valid.end(), card) != valid.end())
                {
                    selectedCard = card;
                    showMessage("Attack card selected. Now choose an enemy target.");
                }
                else showMessage("That card cannot be used as this fighter's attack card.");
                return;
            }

            if (schemeMode && selected)
            {
                const std::vector<int> valid = controller.getGuiSchemeCards(selected);
                if (std::find(valid.begin(), valid.end(), card) != valid.end())
                {
                    const Card chosenCard = current->getDeck()->gethand()[card];
                    if (controller.guiScheme(selected, card))
                    {
                        activeEffectPanel = EffectPanelKind::Scheme;
                        schemeResolved = false;
                        schemeCardName = chosenCard.getName();
                        schemeCardText = chosenCard.geteffect();
                        schemePrompt = "Resolving effect...";
                        schemeChoices.clear();
                        schemeYesNo = false;
                        schemeInteger = false;
                        schemeInputBuffer.clear();
                        schemeMode = false;
                        selectedCharacter = -1;
                        selectedCard = -1;
                        showMessage("Scheme effect is being resolved in the graphic panel.");
                    }
                    else showMessage("That Scheme card cannot be used by this fighter.");
                }
                else showMessage("That card is not a legal Scheme card for this fighter.");
                return;
            }
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

        if (moveMode && awaitingMoveBoost)
        {
            showMessage("Choose YES or NO for the boost first.");
        }
        else
        {
            showMessage(occupant->getName() + " selected.");
        }
        return;
    }

    if (occupant && !controller.isCurrentPlayer(occupant))
    {
        selectedEnemy = space;
        if (attackMode && selected && selectedCard >= 0)
        {
            const std::vector<int> targets = [&]()
            {
                std::vector<int> result;
                for (Character* target : controller.getBord().getAttackCharacters(
                         selected->getAttacktype(), selected->getSpace()))
                {
                    if (target == occupant) result.push_back(space);
                }
                return result;
            }();

            if (targets.empty())
            {
                showMessage("This enemy is not in attack range.");
                return;
            }

            Player* enemyPlayer = controller.getEnemyPlayer();
            if (enemyPlayer && enemyPlayer->isAI())
            {
                const std::vector<int> valid = controller.getGuiDefenseCards(occupant);
                if (valid.empty())
                {
                    showMessage("The defending fighter has no valid defense card.");
                    return;
                }
                if (controller.guiAttack(selected, occupant, selectedCard, valid.front()))
                {
                    const Card attackCard = current->getDeck()->getHandcard(selectedCard);
                    const Card defenseCard = enemyPlayer->getDeck()->gethand()[valid.front()];
                    activeEffectPanel = EffectPanelKind::Attack;
                    attackResolved = false;
                    attackCardName = "COMBAT";
                    attackCardText = attackCard.getName() + "  VS  " + defenseCard.getName() +
                                     "\n\nATTACK EFFECT: " + (attackCard.geteffect().empty() ? "None" : attackCard.geteffect()) +
                                     "\nDEFENSE EFFECT: " + (defenseCard.geteffect().empty() ? "None" : defenseCard.geteffect()) +
                                     "\n\nResolving attack, defense and combat effects...";
                    attackPrompt = "Resolving combat...";
                    attackChoices.clear();
                    attackYesNo = false;
                    attackInteger = false;
                    attackInputBuffer.clear();
                    attackMode = false;
                    selectedCard = -1;
                    selectedEnemy = -1;
                    selectedCharacter = -1;
                    showMessage("Combat is resolving in the graphic panel.");
                }
            }
            else
            {
                defenseSelectionMode = true;
                showMessage(enemyPlayer ? enemyPlayer->getName() + ": choose a defense card." : "Choose a defense card.");
            }
        }
        return;
    }

    if (moveMode && selected)
    {
        const int movement = selected->getMove() + moveBoost;
        const std::vector<int> valid = controller.getValidMoveSpaces(selected, movement);
        if (std::find(valid.begin(), valid.end(), space) == valid.end())
        {
            showMessage("That destination is not legal for this movement.");
            return;
        }

        if (controller.guiMove(selected, movement, space))
        {
            selectedSpace = space;
            moveMode = false;
            selectedCharacter = -1;
            moveBoost = 0;
            showMessage("Movement completed.");
            checkGameOver();
            advanceTurnIfTwoActionsUsed();
        }
    }
}


// ============================================================================
// ATTACK EFFECTS PANEL
// Fully independent from the Scheme and Dracula panels: its own state,
// its own update/render/click-handling.
// ============================================================================

void GameWindow::updateAttackEffectPanel()
{
    std::string prompt;
    std::vector<int> choices;
    bool yesNo = false;
    bool integerInput = false;

    if (controller.getGuiInputRequest(prompt, choices, yesNo, integerInput))
    {
        attackPrompt = prompt;
        attackChoices = choices;
        attackYesNo = yesNo;
        attackInteger = integerInput;
    }
    else if (controller.guiEffectBusy() && !attackResolved && attackPrompt.empty())
    {
        attackPrompt = "Resolving combat effect...";
    }

    if (controller.guiEffectFinished())
    {
        attackResolved = true;
        attackPrompt = "Effect resolved.";
        attackChoices.clear();
        attackYesNo = false;
        attackInteger = false;
        attackInputBuffer.clear();

        refreshCombatLog();
        attackMode = false;
        defenseSelectionMode = false;
        selectedCard = -1;
        selectedEnemy = -1;
        selectedCharacter = -1;

        // The second action ends the turn automatically as soon as the
        // complete combat/effect resolution has finished.
        if (controller.getActionCount() >= 2)
        {
            activeEffectPanel = EffectPanelKind::None;
            attackResolved = false;
            controller.clearGuiEffectLog();
            checkGameOver();
            advanceTurnIfTwoActionsUsed();
        }
    }
}

void GameWindow::drawAttackEffectPanel()
{
    sf::RectangleShape overlay({1600.f, 900.f});
    overlay.setFillColor(sf::Color(2, 3, 7, 175));
    window.draw(overlay);

    const sf::FloatRect panel({330.f, 70.f}, {940.f, 760.f});
    ui->drawPanel(window, panel, GOLD);

    ui->drawText(window, attackCardName.empty() ? "ATTACK EFFECT" : attackCardName,
                 {385.f, 100.f}, 24, GOLD);

    auto drawWrapped = [&](const std::string& text, float x, float y0, float widthChars, int size, sf::Color color, int maxLines)
    {
        std::string line;
        int row = 0;
        const auto flush = [&]()
        {
            if (!line.empty() && row < maxLines)
                ui->drawText(window, line, {x, y0 + row * (size + 5.f)}, size, color);
            line.clear();
            ++row;
        };
        for (char ch : text)
        {
            if (row >= maxLines) break;
            if (ch == '\n') { flush(); continue; }
            line += ch;
            if (line.size() >= static_cast<std::size_t>(widthChars) && ch == ' ')
                flush();
        }
        if (row < maxLines && !line.empty()) flush();
    };

    drawWrapped(attackCardText.empty() ? "Resolving combat..." : attackCardText,
                385.f, 145.f, 94.f, 13, PARCHMENT, 7);

    // Live effect log: every cout-like message generated by the combat/attack
    // effects is shown here.
    const std::vector<std::string> logs = controller.getGuiEffectLog();
    float y = 300.f;
    int shown = 0;
    for (auto it = logs.rbegin(); it != logs.rend() && shown < 11; ++it)
    {
        if (it->empty()) continue;
        std::string line = *it;
        float lineY = y + static_cast<float>(shown) * 25.f;
        std::string chunk;
        int wrapped = 0;
        for (char c : line)
        {
            chunk += c;
            if (chunk.size() >= 82)
            {
                ui->drawText(window, chunk, {385.f, lineY + wrapped * 20.f}, 12, PARCHMENT);
                chunk.clear();
                ++wrapped;
                if (wrapped >= 2) break;
            }
        }
        if (!chunk.empty() && wrapped < 2)
            ui->drawText(window, chunk, {385.f, lineY + wrapped * 20.f}, 12, PARCHMENT);
        ++shown;
    }

    if (!attackPrompt.empty())
        ui->drawText(window, attackPrompt, {385.f, 470.f}, 16, GOLD);

    if (!attackResolved)
    {
        // Graphical input box: wherever the attack/combat effect logic calls
        // for cin (yes/no, a numbered choice, or a typed number), the
        // corresponding control below appears right under the effect log
        // and its value is fed back to the Controller instead of stdin.
        if (attackYesNo)
        {
            ui->drawButton(window, {{470.f, 520.f}, {220.f, 55.f}}, "YES", true, GREEN);
            ui->drawButton(window, {{730.f, 520.f}, {220.f, 55.f}}, "NO", true, RED);
        }
        else if (!attackChoices.empty())
        {
            const float startX = 390.f;
            const float startY = 520.f;
            const float bw = 88.f;
            const float bh = 36.f;
            const float gap = 8.f;

            for (std::size_t i = 0; i < attackChoices.size() && i < 40; ++i)
            {
                const int col = static_cast<int>(i % 8);
                const int row = static_cast<int>(i / 8);
                ui->drawButton(window,
                               {{startX + col * (bw + gap),
                                 startY + row * (bh + gap)},
                                {bw, bh}},
                               std::to_string(attackChoices[i]), true, GOLD);
            }
        }
        else if (attackInteger)
        {
            ui->drawPanel(window, {{470.f, 520.f}, {500.f, 55.f}}, GOLD);
            ui->drawText(window, attackInputBuffer.empty() ? "_" : attackInputBuffer,
                         {490.f, 534.f}, 18, PARCHMENT);
            ui->drawText(window, "Type the number, then press ENTER.",
                         {470.f, 585.f}, 12, PARCHMENT);
        }
    }

    if (attackResolved)
    {
        const bool canClose = controller.guiEffectCanClose();
        ui->drawButton(window, {{565.f, 705.f}, {470.f, 58.f}},
                       "RETURN TO GAME", canClose, GOLD);
    }
}

void GameWindow::handleAttackEffectInput(sf::Vector2f p)
{
    if (attackResolved)
    {
        if (sf::FloatRect({565.f, 705.f}, {470.f, 58.f}).contains(p) && controller.guiEffectCanClose())
        {
            activeEffectPanel = EffectPanelKind::None;
            attackResolved = false;
            attackCardName.clear();
            attackCardText.clear();
            attackPrompt.clear();
            attackChoices.clear();
            attackYesNo = false;
            attackInteger = false;
            attackInputBuffer.clear();
            controller.clearGuiEffectLog();
            checkGameOver();
            advanceTurnIfTwoActionsUsed();
        }
        return;
    }

    if (attackYesNo)
    {
        if (sf::FloatRect({470.f, 520.f}, {220.f, 55.f}).contains(p))
            controller.submitGuiYesNo(true);
        else if (sf::FloatRect({730.f, 520.f}, {220.f, 55.f}).contains(p))
            controller.submitGuiYesNo(false);
        return;
    }

    if (!attackChoices.empty())
    {
        const float startX = 390.f;
        const float startY = 520.f;
        const float bw = 88.f;
        const float bh = 36.f;
        const float gap = 8.f;

        for (std::size_t i = 0; i < attackChoices.size() && i < 40; ++i)
        {
            const int col = static_cast<int>(i % 8);
            const int row = static_cast<int>(i / 8);
            sf::FloatRect button(
                {startX + col * (bw + gap), startY + row * (bh + gap)},
                {bw, bh});

            if (button.contains(p))
            {
                controller.submitGuiInput(attackChoices[i]);
                return;
            }
        }
    }
}

// ============================================================================
// SCHEME EFFECTS PANEL
// Fully independent from the Attack and Dracula panels: its own state,
// its own update/render/click-handling.
// ============================================================================

void GameWindow::updateSchemeEffectPanel()
{
    std::string prompt;
    std::vector<int> choices;
    bool yesNo = false;
    bool integerInput = false;

    if (controller.getGuiInputRequest(prompt, choices, yesNo, integerInput))
    {
        schemePrompt = prompt;
        schemeChoices = choices;
        schemeYesNo = yesNo;
        schemeInteger = integerInput;
    }
    else if (controller.guiEffectBusy() && !schemeResolved && schemePrompt.empty())
    {
        schemePrompt = "Resolving scheme effect...";
    }

    if (controller.guiEffectFinished())
    {
        schemeResolved = true;
        schemePrompt = "Effect resolved.";
        schemeChoices.clear();
        schemeYesNo = false;
        schemeInteger = false;
        schemeInputBuffer.clear();

        schemeMode = false;
        selectedCard = -1;
        selectedCharacter = -1;

        if (controller.getActionCount() >= 2)
        {
            activeEffectPanel = EffectPanelKind::None;
            schemeResolved = false;
            controller.clearGuiEffectLog();
            checkGameOver();
            advanceTurnIfTwoActionsUsed();
        }
    }
}

void GameWindow::drawSchemeEffectPanel()
{
    sf::RectangleShape overlay({1600.f, 900.f});
    overlay.setFillColor(sf::Color(2, 3, 7, 175));
    window.draw(overlay);

    const sf::FloatRect panel({330.f, 70.f}, {940.f, 760.f});
    ui->drawPanel(window, panel, GOLD);

    ui->drawText(window, schemeCardName.empty() ? "SCHEME EFFECT" : schemeCardName,
                 {385.f, 100.f}, 24, GOLD);

    auto drawWrapped = [&](const std::string& text, float x, float y0, float widthChars, int size, sf::Color color, int maxLines)
    {
        std::string line;
        int row = 0;
        const auto flush = [&]()
        {
            if (!line.empty() && row < maxLines)
                ui->drawText(window, line, {x, y0 + row * (size + 5.f)}, size, color);
            line.clear();
            ++row;
        };
        for (char ch : text)
        {
            if (row >= maxLines) break;
            if (ch == '\n') { flush(); continue; }
            line += ch;
            if (line.size() >= static_cast<std::size_t>(widthChars) && ch == ' ')
                flush();
        }
        if (row < maxLines && !line.empty()) flush();
    };

    drawWrapped(schemeCardText.empty() ? "Resolving Scheme effect..." : schemeCardText,
                385.f, 145.f, 94.f, 13, PARCHMENT, 7);

    // Live effect log: every cout-like message generated by the Scheme
    // effect is shown here.
    const std::vector<std::string> logs = controller.getGuiEffectLog();
    float y = 300.f;
    int shown = 0;
    for (auto it = logs.rbegin(); it != logs.rend() && shown < 11; ++it)
    {
        if (it->empty()) continue;
        std::string line = *it;
        float lineY = y + static_cast<float>(shown) * 25.f;
        std::string chunk;
        int wrapped = 0;
        for (char c : line)
        {
            chunk += c;
            if (chunk.size() >= 82)
            {
                ui->drawText(window, chunk, {385.f, lineY + wrapped * 20.f}, 12, PARCHMENT);
                chunk.clear();
                ++wrapped;
                if (wrapped >= 2) break;
            }
        }
        if (!chunk.empty() && wrapped < 2)
            ui->drawText(window, chunk, {385.f, lineY + wrapped * 20.f}, 12, PARCHMENT);
        ++shown;
    }

    if (!schemePrompt.empty())
        ui->drawText(window, schemePrompt, {385.f, 470.f}, 16, GOLD);

    if (!schemeResolved)
    {
        // Graphical input box: wherever the Scheme effect logic calls for
        // cin (yes/no, a numbered choice, or a typed number), the
        // corresponding control below appears right under the effect log
        // and its value is fed back to the Controller instead of stdin.
        if (schemeYesNo)
        {
            ui->drawButton(window, {{470.f, 520.f}, {220.f, 55.f}}, "YES", true, GREEN);
            ui->drawButton(window, {{730.f, 520.f}, {220.f, 55.f}}, "NO", true, RED);
        }
        else if (!schemeChoices.empty())
        {
            const float startX = 390.f;
            const float startY = 520.f;
            const float bw = 88.f;
            const float bh = 36.f;
            const float gap = 8.f;

            for (std::size_t i = 0; i < schemeChoices.size() && i < 40; ++i)
            {
                const int col = static_cast<int>(i % 8);
                const int row = static_cast<int>(i / 8);
                ui->drawButton(window,
                               {{startX + col * (bw + gap),
                                 startY + row * (bh + gap)},
                                {bw, bh}},
                               std::to_string(schemeChoices[i]), true, GOLD);
            }
        }
        else if (schemeInteger)
        {
            ui->drawPanel(window, {{470.f, 520.f}, {500.f, 55.f}}, GOLD);
            ui->drawText(window, schemeInputBuffer.empty() ? "_" : schemeInputBuffer,
                         {490.f, 534.f}, 18, PARCHMENT);
            ui->drawText(window, "Type the number, then press ENTER.",
                         {470.f, 585.f}, 12, PARCHMENT);
        }
    }

    if (schemeResolved)
    {
        const bool canClose = controller.guiEffectCanClose();
        ui->drawButton(window, {{565.f, 705.f}, {470.f, 58.f}},
                       "RETURN TO GAME", canClose, GOLD);
    }
}

void GameWindow::handleSchemeEffectInput(sf::Vector2f p)
{
    if (schemeResolved)
    {
        if (sf::FloatRect({565.f, 705.f}, {470.f, 58.f}).contains(p) && controller.guiEffectCanClose())
        {
            activeEffectPanel = EffectPanelKind::None;
            schemeResolved = false;
            schemeCardName.clear();
            schemeCardText.clear();
            schemePrompt.clear();
            schemeChoices.clear();
            schemeYesNo = false;
            schemeInteger = false;
            schemeInputBuffer.clear();
            controller.clearGuiEffectLog();
            checkGameOver();
            advanceTurnIfTwoActionsUsed();
        }
        return;
    }

    if (schemeYesNo)
    {
        if (sf::FloatRect({470.f, 520.f}, {220.f, 55.f}).contains(p))
            controller.submitGuiYesNo(true);
        else if (sf::FloatRect({730.f, 520.f}, {220.f, 55.f}).contains(p))
            controller.submitGuiYesNo(false);
        return;
    }

    if (!schemeChoices.empty())
    {
        const float startX = 390.f;
        const float startY = 520.f;
        const float bw = 88.f;
        const float bh = 36.f;
        const float gap = 8.f;

        for (std::size_t i = 0; i < schemeChoices.size() && i < 40; ++i)
        {
            const int col = static_cast<int>(i % 8);
            const int row = static_cast<int>(i / 8);
            sf::FloatRect button(
                {startX + col * (bw + gap), startY + row * (bh + gap)},
                {bw, bh});

            if (button.contains(p))
            {
                controller.submitGuiInput(schemeChoices[i]);
                return;
            }
        }
    }
}

// ============================================================================
// DRACULA SPECIAL-ABILITY PANEL
// Fully independent from the Attack and Scheme panels: its own state,
// its own update/render/click-handling. Unlike the other two, its title
// and body text are fixed, and its RETURN TO GAME button stays visible
// (disabled) from the moment the panel opens rather than only once resolved.
// ============================================================================

void GameWindow::updateDraculaEffectPanel()
{
    if (controller.guiEffectFinished())
    {
        draculaResolved = true;
        draculaPrompt = "Effect resolved. Press RETURN TO GAME to continue.";
        draculaChoices.clear();
        draculaYesNo = false;
        draculaInteger = false;
        draculaInputBuffer.clear();
        return;
    }

    if (draculaResolved)
        return;

    // Always mirror the Controller's current request. There is deliberately
    // no fake/fallback Yes/No request here: the buttons must be backed by an
    // actual pending request so every click has a real consumer.
    std::string prompt;
    std::vector<int> choices;
    bool yesNo = false;
    bool integerInput = false;

    if (controller.getGuiInputRequest(prompt, choices, yesNo, integerInput))
    {
        draculaPrompt = prompt;
        draculaChoices = choices;
        draculaYesNo = yesNo;
        draculaInteger = integerInput;
    }
    else if (controller.guiEffectBusy() && draculaChoices.empty() && !draculaInteger && !draculaYesNo)
    {
        draculaYesNo = true;
        draculaPrompt = "Use Dracula's ability?";
    }
}

void GameWindow::drawDraculaEffectPanel()
{
    sf::RectangleShape overlay({1600.f, 900.f});
    overlay.setFillColor(sf::Color(2, 3, 7, 175));
    window.draw(overlay);

    const sf::FloatRect panel({330.f, 70.f}, {940.f, 760.f});
    ui->drawPanel(window, panel, GOLD);

    Player* abilityPlayer = controller.getCurrentPlayer();
    const std::string heroName = (abilityPlayer && abilityPlayer->getHero())
        ? abilityPlayer->getHero()->getName() : "";

    std::string abilityTitle = "HERO ABILITY";
    std::string abilityBody;
    if (heroName == "Dracula")
    {
        abilityTitle = "DRACULA ABILITY";
        abilityBody = "At the beginning of Dracula's turn, choose one adjacent living fighter (including a Sister) to take 1 damage. If damage is dealt, draw 1 card.";
    }
    else if (heroName == "sherlock")
    {
        abilityTitle = "SHERLOCK HOLMES ABILITY";
        abilityBody = "PASSIVE: effects that try to disable Sherlock or Dr. Watson-related cards do not disable those cards.";
    }
    else if (heroName == "invisible man")
    {
        abilityTitle = "INVISIBLE MAN ABILITY";
        abilityBody = "PASSIVE: while defending on fog, defense is +1 (not a card effect). Invisible Man may also move directly between fog spaces.";
    }

    ui->drawText(window, abilityTitle, {385.f, 100.f}, 24, GOLD);
    ui->drawText(window, abilityBody, {385.f, 145.f}, 14, PARCHMENT);

    // Live effect log: every cout-like message generated by Dracula's
    // ability is shown here.
    const std::vector<std::string> logs = controller.getGuiEffectLog();
    float y = 190.f;
    int shown = 0;
    for (auto it = logs.rbegin(); it != logs.rend() && shown < 11; ++it)
    {
        if (it->empty()) continue;
        std::string line = *it;
        float lineY = y + static_cast<float>(shown) * 25.f;
        std::string chunk;
        int wrapped = 0;
        for (char c : line)
        {
            chunk += c;
            if (chunk.size() >= 82)
            {
                ui->drawText(window, chunk, {385.f, lineY + wrapped * 20.f}, 12, PARCHMENT);
                chunk.clear();
                ++wrapped;
                if (wrapped >= 2) break;
            }
        }
        if (!chunk.empty() && wrapped < 2)
            ui->drawText(window, chunk, {385.f, lineY + wrapped * 20.f}, 12, PARCHMENT);
        ++shown;
    }

    if (!draculaPrompt.empty())
    {
        std::string text = draculaPrompt;
        std::string line;
        int row = 0;
        for (char ch : text)
        {
            line += ch;
            if (line.size() >= 88 && ch == ' ')
            {
                ui->drawText(window, line, {385.f, 460.f + row * 20.f}, 14, GOLD);
                line.clear();
                ++row;
                if (row >= 3) break;
            }
        }
        if (!line.empty() && row < 3)
            ui->drawText(window, line, {385.f, 460.f + row * 20.f}, 14, GOLD);
    }

    if (!draculaResolved)
    {
        // Graphical input box: wherever Dracula's ability calls for cin
        // (yes/no to use the ability, or a numbered target choice), the
        // corresponding control below appears right under the effect log
        // and its value is fed back to the Controller instead of stdin.
        if (draculaYesNo)
        {
            ui->drawButton(window, {{470.f, 520.f}, {220.f, 55.f}}, "YES", true, GREEN);
            ui->drawButton(window, {{730.f, 520.f}, {220.f, 55.f}}, "NO", true, RED);
        }
        else if (!draculaChoices.empty())
        {
            const float startX = 390.f;
            const float startY = 520.f;
            const float bw = 88.f;
            const float bh = 36.f;
            const float gap = 8.f;

            for (std::size_t i = 0; i < draculaChoices.size() && i < 40; ++i)
            {
                const int col = static_cast<int>(i % 8);
                const int row = static_cast<int>(i / 8);
                ui->drawButton(window,
                               {{startX + col * (bw + gap),
                                 startY + row * (bh + gap)},
                                {bw, bh}},
                               std::to_string(draculaChoices[i]), true, GOLD);
            }
        }
        else if (draculaInteger)
        {
            ui->drawPanel(window, {{470.f, 520.f}, {500.f, 55.f}}, GOLD);
            ui->drawText(window, draculaInputBuffer.empty() ? "_" : draculaInputBuffer,
                         {490.f, 534.f}, 18, PARCHMENT);
            ui->drawText(window, "Type the number, then press ENTER.",
                         {470.f, 585.f}, 12, PARCHMENT);
        }
    }

    // Dracula's special-ability panel keeps the return button visible from
    // the moment the panel opens. It becomes clickable as soon as the
    // controller says the effect may be closed.
    const bool canClose = draculaResolved && controller.guiEffectCanClose();
    ui->drawButton(window, {{565.f, 705.f}, {470.f, 58.f}},
                   "RETURN TO GAME", canClose, GOLD);
}

void GameWindow::handleDraculaEffectInput(sf::Vector2f p)
{
    if (sf::FloatRect({565.f, 705.f}, {470.f, 58.f}).contains(p))
    {
        if (controller.guiEffectCanClose())
        {
            activeEffectPanel = EffectPanelKind::None;
            draculaResolved = false;
            draculaPrompt.clear();
            draculaChoices.clear();
            draculaYesNo = false;
            draculaInteger = false;
            draculaInputBuffer.clear();
            controller.clearGuiEffectLog();
            checkGameOver();
        }
        return;
    }

    if (draculaResolved)
        return;

    if (draculaYesNo)
    {
        const bool clickedYes = sf::FloatRect({470.f, 520.f}, {220.f, 55.f}).contains(p);
        const bool clickedNo  = sf::FloatRect({730.f, 520.f}, {220.f, 55.f}).contains(p);

        if (clickedYes || clickedNo)
        {
            // Query the Controller at click time so the UI cannot submit a
            // stale request from a previous frame.
            std::string prompt;
            std::vector<int> choices;
            bool yesNo = false;
            bool integerInput = false;
            if (controller.getGuiInputRequest(prompt, choices, yesNo, integerInput) && yesNo)
                controller.submitGuiYesNo(clickedYes);
        }
        return;
    }

    if (!draculaChoices.empty())
    {
        const float startX = 390.f;
        const float startY = 520.f;
        const float bw = 88.f;
        const float bh = 36.f;
        const float gap = 8.f;

        for (std::size_t i = 0; i < draculaChoices.size() && i < 40; ++i)
        {
            const int col = static_cast<int>(i % 8);
            const int row = static_cast<int>(i / 8);
            sf::FloatRect button(
                {startX + col * (bw + gap), startY + row * (bh + gap)},
                {bw, bh});

            if (button.contains(p))
            {
                controller.submitGuiInput(draculaChoices[i]);
                return;
            }
        }
    }
}

void GameWindow::advanceTurnIfTwoActionsUsed()
{
    if (controller.getActionCount() < 2)
        return;

    Player* player = controller.getCurrentPlayer();
    if (player && player->getDeck() && player->getDeck()->gethandSize() > 7)
    {
        handLimitMode = true;
        showMessage("Both actions are complete. Discard down to 7 cards to end the turn.");
        return;
    }

    controller.guiEndTurn();
    resetSelections();
    combatLog.clear();
    controller.guiBeginTurn();
    showMessage("Both actions are complete. Turn changed.");
}

void GameWindow::finishTurnAfterHandLimit()
{
    handLimitMode = false;
    controller.guiEndTurn();
    resetSelections();
    combatLog.clear();
    controller.guiBeginTurn();
    showMessage("Turn changed.");
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
    schemeMode = false;
    defenseSelectionMode = false;
    awaitingMoveBoost = false;
    moveBoostPrompt = false;
    moveBoost = 0;

    activeEffectPanel = EffectPanelKind::None;

    attackCardName.clear();
    attackCardText.clear();
    attackPrompt.clear();
    attackChoices.clear();
    attackYesNo = false;
    attackInteger = false;
    attackResolved = false;
    attackInputBuffer.clear();

    schemeCardName.clear();
    schemeCardText.clear();
    schemePrompt.clear();
    schemeChoices.clear();
    schemeYesNo = false;
    schemeInteger = false;
    schemeResolved = false;
    schemeInputBuffer.clear();

    draculaPrompt.clear();
    draculaChoices.clear();
    draculaYesNo = false;
    draculaInteger = false;
    draculaResolved = false;
    draculaInputBuffer.clear();

    handLimitMode = false;
}

Character* GameWindow::selectedCurrentCharacter() const
{
    Player* current = controller.getCurrentPlayer();
    if (!current || selectedCharacter < 0 || selectedCharacter >= current->getfighterCount())
        return nullptr;
    return current->getFighter(selectedCharacter);
}