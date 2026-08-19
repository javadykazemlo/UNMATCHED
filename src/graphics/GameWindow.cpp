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

    std::string cardChoiceLabel(const Player* player, int choice, bool oneBased)
    {
        if (!player || !player->getDeck()) return std::to_string(choice);

        const int index = oneBased ? choice - 1 : choice;
        if (index < 0 || index >= player->getDeck()->gethandSize())
            return std::to_string(choice);

        return player->getDeck()->getHandcard(index).getName();
    }

    std::string characterChoiceLabel(Player* player, int choice)
    {
        if (!player || choice < 1) return std::to_string(choice);

        int aliveIndex = 0;
        for (Character* character : player->getCharacters())
        {
            if (!character || !character->checkalive()) continue;
            ++aliveIndex;
            if (aliveIndex == choice)
                return character->getName();
        }

        return std::to_string(choice);
    }

    std::string choiceContextLabel(const std::string& context,
                                     Player* player,
                                    int choice)
    {
        std::string lower = context;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if (lower.find("card") != std::string::npos)
            return cardChoiceLabel(player, choice, true);

        if (lower.find("character") != std::string::npos ||
            lower.find("fighter") != std::string::npos ||
            lower.find("target") != std::string::npos)
            return characterChoiceLabel(player, choice);

        return std::to_string(choice);
    }
}

GameWindow::GameWindow()
    : window(sf::VideoMode({1600u, 900u}), "UNMATCHED")
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
    textures.load("end", "assets/backgrounds/end.png");
    textures.load("ready", "assets/backgrounds/ready.png");
    textures.load("board", "assets/board/board.png");
    textures.load("card_back", "assets/cards/card_back.png");
    textures.load("mute", "assets/mute.png");

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

    textures.load("panel_dracul", "assets/characters/panel_dracula.png");
    textures.load("panel_sherlok", "assets/characters/panel_sherlock.png");
    textures.load("panel_invisible", "assets/characters/panel_invisible.png");

    textures.load("dracula_prof", "assets/characters/dracula_prof.png");
    textures.load("sherlock_prof", "assets/characters/sherlock_prof.png");
    textures.load("invisible_prof", "assets/characters/invisible_prof.png");
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

        if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
        {
            const sf::Vector2f mp = window.mapPixelToCoords(mouse->position);
            if (screen == Screen::MainMenu && !exitConfirmPopup &&
                !(rulesView && rulesView->isOpen()))
            {
                int newHover = -1;
                for (int i = 0; i < 4; ++i)
                {
                    if (sf::FloatRect({575.f, 425.f + i * 80.f},
                                      {450.f, 60.f}).contains(mp))
                    {
                        newHover = i;
                        break;
                    }
                }

                if (newHover != hoveredMenuItem)
                {
                    if (newHover >= 0)
                        audio.playSfx(AudioManager::Sfx::Hover);
                    hoveredMenuItem = newHover;
                }
            }
            else if (screen == Screen::Setup &&
                     controller.getGuiSetupStage() ==
                         Controller::GuiSetupStage::CharacterSelection)
            {
                int newHover = -1;
                for (int i = 0; i < 3; ++i)
                {
                    if (sf::FloatRect({330.f + i * 330.f, 225.f},
                                      {280.f, 350.f}).contains(mp))
                    {
                        newHover = i;
                        break;
                    }
                }

                if (newHover != hoveredMenuItem)
                {
                    if (newHover >= 0)
                        audio.playSfx(AudioManager::Sfx::Hover);
                    hoveredMenuItem = newHover;
                }
            }
            else if (screen != Screen::MainMenu)
            {
                hoveredMenuItem = -1;
            }
        }

        if (rulesView && rulesView->isOpen())
        {
            sf::Vector2f mousePosition{0.f, 0.f};
            if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
                mousePosition = window.mapPixelToCoords(mouse->position);
            else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
                mousePosition = window.mapPixelToCoords(mouse->position);

            rulesView->handleEvent(*event, mousePosition);
            continue;
        }

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
        else if (screen == Screen::Game && activeEffectPanel == EffectPanelKind::AiTurn && aiTurnInteger)
        {
            if (const auto* text = event->getIf<sf::Event::TextEntered>())
            {
                if (text->unicode >= '0' && text->unicode <= '9' &&
                    aiTurnInputBuffer.size() < 3)
                    aiTurnInputBuffer.push_back(static_cast<char>(text->unicode));
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Backspace && !aiTurnInputBuffer.empty())
                    aiTurnInputBuffer.pop_back();
                else if (key->code == sf::Keyboard::Key::Enter && !aiTurnInputBuffer.empty())
                {
                    if (controller.submitGuiInput(std::stoi(aiTurnInputBuffer)))
                        aiTurnInputBuffer.clear();
                }
            }
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (screen == Screen::Game && saveSlotPopup &&
                key->code == sf::Keyboard::Key::Escape)
            {
                saveSlotPopup = false;
                continue;
            }

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
            audio.playSfx(AudioManager::Sfx::Click);
            const sf::Vector2f p = window.mapPixelToCoords(mouse->position);

            if (rulesView && rulesView->isOpen())
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
                if (exitConfirmPopup)
                {
                    if (sf::FloatRect({650.f, 520.f}, {130.f, 48.f}).contains(p))
                    {
                        window.close();
                        return;
                    }
                    if (sf::FloatRect({820.f, 520.f}, {130.f, 48.f}).contains(p))
                    {
                        exitConfirmPopup = false;
                        return;
                    }
                }
                else if (sf::FloatRect({610.f, 545.f}, {380.f, 50.f}).contains(p))
                {
                    screen = Screen::MainMenu;
                    audio.playIntroMusic();
                    audio.playSfx(AudioManager::Sfx::Confirm);
                }
                else if (sf::FloatRect({610.f, 610.f}, {380.f, 50.f}).contains(p))
                {
                    exitConfirmPopup = true;
                }
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
            case EffectPanelKind::AiTurn:  updateAiTurnPanel();        break;
            case EffectPanelKind::None:    break;
        }

        if (activeEffectPanel == EffectPanelKind::None)
            startAiTurnIfNeeded();

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

    if (rulesView && rulesView->isOpen())
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

    const float t = menuAnimationClock.getElapsedTime().asSeconds();
    for (int i = 0; i < 8; ++i)
    {
        const float phase = t * (0.35f + i * 0.025f) + i * 0.9f;
        sf::CircleShape glow(3.f + (std::sin(phase) + 1.f) * 2.f);
        glow.setOrigin({glow.getRadius(), glow.getRadius()});
        glow.setPosition({
            190.f + i * 175.f + std::sin(phase * 0.8f) * 35.f,
            150.f + (i % 4) * 170.f + std::cos(phase) * 28.f
        });
        glow.setFillColor(sf::Color(211, 178, 104, 45));
        window.draw(glow);
    }

    ui->drawText(window, "THE ETERNAL BATTLE OF SHADOWS", {670.f, 270.f}, 14,
                 sf::Color(157, 126, 69));
    ui->drawText(window, "UNMATCHED", {600.f, 305.f}, 58, GOLD);

    const std::vector<std::string> labels = {"START GAME", "LOAD GAME", "RULES", "EXIT"};
    const std::vector<sf::Color> accents = {GOLD, RED, RED, RED};
    const float baseY = 425.f;

    for (int i = 0; i < 4; ++i)
    {
        const bool hovered = hoveredMenuItem == i;
        const float scale = hovered ? 1.045f : 1.f;
        const float w = 450.f * scale;
        const float h = 60.f * scale;
        const float x = 800.f - w / 2.f;
        const float y = baseY + i * 80.f - (h - 60.f) / 2.f - (hovered ? 3.f : 0.f);

        sf::FloatRect rect({x, y}, {w, h});
        ui->drawButton(window, rect, labels[i], true, accents[i]);

        if (hovered)
        {
            sf::RectangleShape lift({w + 10.f, h + 10.f});
            lift.setPosition({x - 5.f, y - 5.f});
            lift.setFillColor(sf::Color::Transparent);
            lift.setOutlineColor(sf::Color(211, 178, 104, 70));
            lift.setOutlineThickness(2.f);
            window.draw(lift);
        }
    }

    ui->drawText(window, "created & Developed by", {700.f, 750.f}, 14,
                 sf::Color(170, 162, 150));
    ui->drawText(window, "Mahdi Dehnavi & Mohammd Javad Kazemlo",
                 {640.f, 772.f}, 14, sf::Color(170, 162, 150));

    if (exitConfirmPopup)
    {
        sf::RectangleShape dim({1600.f, 900.f});
        dim.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(dim);

        ui->drawPanel(window, {{500.f, 365.f}, {600.f, 245.f}}, GOLD);
        ui->drawText(window, "ARE YOU SURE YOU WANT TO EXIT?",
                     {605.f, 415.f}, 20, PARCHMENT);
        ui->drawButton(window, {{650.f, 520.f}, {130.f, 48.f}}, "YES", true, RED);
        ui->drawButton(window, {{820.f, 520.f}, {130.f, 48.f}}, "NO", true, GOLD);
    }
}

void GameWindow::refreshSaveEntries()
{
    namespace fs = std::filesystem;
    using json = nlohmann::json;

    saveEntries.clear();

    for (int slot = 1; slot <= 3; ++slot)
    {
        SaveEntry info;
        info.slot = slot;

        std::error_code ec;
        const fs::path path = fs::absolute(
            fs::path("saves") / ("slot_" + std::to_string(slot) + ".json"), ec);

        if (ec)
            continue;

        info.path = path.string();

        if (!fs::exists(path, ec) || !fs::is_regular_file(path, ec))
        {
            saveEntries.push_back(std::move(info));
            continue;
        }

        try
        {
            std::ifstream file(path);
            if (!file.is_open())
            {
                saveEntries.push_back(std::move(info));
                continue;
            }

            json root;
            file >> root;

            if (!root.contains("players") ||
                !root["players"].is_array() ||
                root["players"].size() < 2)
            {
                saveEntries.push_back(std::move(info));
                continue;
            }

            info.timestamp = 0;

            if (root.contains("saveMetadata") && root["saveMetadata"].is_object())
            {
                const auto& meta = root.at("saveMetadata");
                info.date = meta.value("date", "");
                info.time = meta.value("time", "");
                info.timestamp = meta.value("timestamp", 0LL);
            }

            if (info.timestamp == 0)
            {
                const auto last = fs::last_write_time(path, ec);
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
                    std::chrono::system_clock::time_point(
                        std::chrono::milliseconds(info.timestamp)));

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

            const auto& p0 = root["players"][0];
            const auto& p1 = root["players"][1];

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
        }
        catch (...)
        {
        }

        saveEntries.push_back(std::move(info));
    }
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
    ui->drawText(window, "SELECT ONE OF THE THREE SAVE SLOTS.",
                 {650.f, 185.f}, 10, PARCHMENT);

    const float rowHeight = 118.f;
    for (std::size_t i = 0; i < saveEntries.size() && i < 3; ++i)
    {
        const SaveEntry& save = saveEntries[i];
        const float y = 230.f + static_cast<float>(i) * rowHeight;

        sf::RectangleShape row({1110.f, 98.f});
        row.setPosition({245.f, y});
        row.setFillColor(sf::Color(15, 16, 23, 245));
        row.setOutlineColor(sf::Color(74, 65, 50));
        row.setOutlineThickness(1.f);
        window.draw(row);

        ui->drawText(window, "SLOT " + std::to_string(save.slot),
                     {270.f, y + 12.f}, 16, GOLD);

        if (save.date.empty())
        {
            ui->drawText(window, "EMPTY SLOT",
                         {270.f, y + 48.f}, 14, PARCHMENT);
        }
        else
        {
            ui->drawText(window, save.heroes,
                         {270.f, y + 42.f}, 13, PARCHMENT);
            ui->drawText(window,
                         save.player1 + "  vs  " + save.player2 +
                         "    |    " + save.date + "  " + save.time,
                         {270.f, y + 67.f}, 9,
                         sf::Color(155, 149, 138));
        }

        ui->drawButton(window, {{1130.f, y + 25.f}, {185.f, 48.f}},
                       "LOAD", !save.date.empty(), GOLD);
    }

    ui->drawButton(window, {{745.f, 755.f}, {175.f, 45.f}},
                   "BACK", false, GOLD);
}

void GameWindow::handleLoadGameClick(sf::Vector2f p)
{
    if (sf::FloatRect({745.f, 755.f}, {175.f, 45.f}).contains(p))
    {
        screen = Screen::MainMenu;
        loadScroll = 0.f;
        return;
    }

    const float rowHeight = 118.f;

    for (std::size_t i = 0; i < saveEntries.size() && i < 3; ++i)
    {
        const SaveEntry& save = saveEntries[i];
        const float y = 230.f + static_cast<float>(i) * rowHeight;

        if (!sf::FloatRect({245.f, y}, {1110.f, 98.f}).contains(p))
            continue;

        if (save.date.empty())
            return;

        players[0].reset();
        players[1].reset();
        controller = Controller();

        if (!controller.LoadGame(players, save.path))
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

        audio.playGameplayMusic();
        audio.playSfx(AudioManager::Sfx::TurnStart);
        return;
    }
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
    ui->drawText(window, "THE PLAYERS", {680.f, 62.f}, 32, GOLD);
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

    ui->drawPanel(window, {{250.f, 125.f}, {1100.f, 555.f}}, GOLD);
    if (chooser)
    {
        ui->drawText(window, chooser->getName() + " - YOUR CHOICE",
                     {660.f, 165.f}, 18,
                     chooser == current ? RED : BLUE);
    }

    const int heroes[] = {1, 2, 3};
    const char* names[] = {"DRACULA", "SHERLOCK", "INVISIBLE MAN"};
    const char* portraits[] = {"dracula_prof", "sherlock_prof", "invisible_prof"};

    const std::vector<int> choices = controller.getGuiCharacterChoices();
    for (int i = 0; i < 3; ++i)
    {
        const bool allowed = std::find(choices.begin(), choices.end(), heroes[i]) != choices.end();
        const sf::FloatRect baseRect({330.f + i * 330.f, 225.f}, {280.f, 350.f});
        const bool hovered = allowed && hoveredMenuItem == i;

        const float scale = hovered ? 1.045f : 1.f;
        const float w = baseRect.size.x * scale;
        const float h = baseRect.size.y * scale;
        const float x = baseRect.position.x - (w - baseRect.size.x) / 2.f;
        const float y = baseRect.position.y - (h - baseRect.size.y) / 2.f -
                        (hovered ? 3.f : 0.f);
        const sf::FloatRect rect({x, y}, {w, h});

        ui->drawButton(window, rect, names[i], allowed,
                       allowed ? GOLD : sf::Color(70, 65, 58));

        if (hovered)
        {
            sf::RectangleShape lift({w + 10.f, h + 10.f});
            lift.setPosition({x - 5.f, y - 5.f});
            lift.setFillColor(sf::Color::Transparent);
            lift.setOutlineColor(sf::Color(211, 178, 104, 70));
            lift.setOutlineThickness(2.f);
            window.draw(lift);
        }

        if (allowed)
        {
            if (const sf::Texture* tex = textures.get(portraits[i]))
            {
                sf::Sprite sprite(*tex);
                const sf::Vector2u size = tex->getSize();
                const float portraitScale =
                    std::min(rect.size.x / static_cast<float>(size.x),
                             rect.size.y / static_cast<float>(size.y));
                sprite.setScale({portraitScale, portraitScale});
                sprite.setPosition({rect.position.x, rect.position.y});
                window.draw(sprite);
            }
        }
    }

    ui->drawText(window, current ? "The younger player chooses first." : "",
                 {692.f, 650.f}, 11, sf::Color(165, 157, 145));
}

void GameWindow::drawSetupPosition()
{
    drawFullscreenTexture("setup");
    ui->drawText(window, "CHOOSE STARTING SIDE", {600.f, 70.f}, 30, GOLD);

    Player* chooser = controller.getGuiSetupPlayer();
    ui->drawPanel(window, {{250.f, 140.f}, {1100.f, 550.f}}, GOLD);

    if (chooser)
        ui->drawText(window, chooser->getName() + " chooses the starting side.",
                     {590.f, 175.f}, 17, PARCHMENT);

    ui->drawButton(window, {{350.f, 270.f}, {390.f, 260.f}},
                   "", true, BLUE);
    ui->drawButton(window, {{860.f, 270.f}, {390.f, 260.f}},
                   "", true, BLUE);

    ui->drawText(window, "LEFT", {515.f, 390.f}, 25, BLUE);
    ui->drawText(window, "RIGHT", {1015.f, 390.f}, 25, BLUE);


    ui->drawText(window, "Your hero starts on space 4.", {430.f, 555.f}, 12, PARCHMENT);
    ui->drawText(window, "Your opponent starts on space 15.", {430.f, 585.f}, 12, PARCHMENT);
}

void GameWindow::drawSetupSidekicks()
{
    drawFullscreenTexture("setup");

    Player* player = controller.getGuiSetupPlayer();
    if (!player) return;

    const bool placingMistTokens =
        player->getHero() &&
        player->getHero()->getName() == "invisible man";

    ui->drawText(window,
        placingMistTokens ? "PLACE FOG TOKENS" : "PLACE YOUR FIGHTERS",
        {630.f, 40.f}, 30, GOLD);

    ui->drawPanel(window, {{245.f, 705.f}, {1110.f, 120.f}}, GOLD);
    ui->drawText(window,
        player->getName() + (placingMistTokens
            ? " - choose a starting space for fog token #" +
              std::to_string(controller.guiSidekickIndex)
            : " - choose a starting space"),
        {510.f, 745.f}, 17, PARCHMENT);

    ui->drawText(window,
        placingMistTokens
            ? "Place each of your 3 fog tokens in your hero's starting zone."
            : "Only spaces in your hero's starting zone are legal.",
        {510.f, 778.f}, 11, sf::Color(165, 157, 145));

    std::vector<int> valid = controller.getGuiPlacementSpaces();
    boardView->draw(window, controller.getBord(), -1, valid);

    for (int pos : valid)
    {
        const sf::Vector2f p = boardView->getPosition(pos);
        ui->drawText(window, placingMistTokens ? "FOG" : "PLACE",
                     {p.x - (placingMistTokens ? 12.f : 20.f), p.y - 43.f},
                     7, GOLD);
    }
}

void GameWindow::drawSetupReady()
{
    drawFullscreenTexture("ready");

    ui->drawText(window, "Players, characters and starting positions are set.",
                 {550.f, 350.f}, 17, sf::Color(224, 218, 200));
    ui->drawText(window, "The game board is now controlled by the real Controller.",
                 {580.f, 385.f}, 13, sf::Color(170, 162, 150));

    ui->drawButton(window, {{520.f, 550.f}, {560.f, 62.f}},
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

    const sf::FloatRect muteRect({305.f, 13.f}, {42.f, 42.f});
    if (const sf::Texture* muteTexture = textures.get("mute"))
    {
        sf::RectangleShape muteBackground(muteRect.size);
        muteBackground.setPosition(muteRect.position);
        muteBackground.setFillColor(audio.isMusicMuted()
                                        ? sf::Color(55, 25, 28, 230)
                                        : sf::Color(18, 18, 25, 220));
        muteBackground.setOutlineColor(audio.isMusicMuted() ? RED : GOLD);
        muteBackground.setOutlineThickness(1.5f);
        window.draw(muteBackground);

        sf::Sprite muteSprite(*muteTexture);
        const sf::Vector2u muteSize = muteTexture->getSize();
        if (muteSize.x > 0 && muteSize.y > 0)
        {
            const float scale = std::min(30.f / static_cast<float>(muteSize.x),
                                         30.f / static_cast<float>(muteSize.y));
            muteSprite.setScale({scale, scale});
            const sf::FloatRect bounds = muteSprite.getGlobalBounds();
            muteSprite.setPosition(
                {muteRect.position.x + (muteRect.size.x - bounds.size.x) / 2.f,
                 muteRect.position.y + (muteRect.size.y - bounds.size.y) / 2.f});
            muteSprite.setColor(audio.isMusicMuted()
                                    ? sf::Color(145, 135, 125)
                                    : sf::Color::White);
            window.draw(muteSprite);
        }
    }
    Player* current = controller.getCurrentPlayer();
    Player* enemy = controller.getEnemyPlayer();
    auto characterColor = [&](Character* character) -> sf::Color
    {
        if (!character) return GOLD;

        const std::string& name = character->getName();
        if (name == "Dracula" || name.find("Sister") != std::string::npos)
            return RED;
        if (name == "sherlock" || name == "Dr_watson")
            return GOLD;
        if (name == "invisible man")
            return sf::Color(86, 101, 115);

        return GOLD;
    };

    const sf::Color turnColor = current ? characterColor(current->getHero()) : GOLD;
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
        if (!hero) return;

        const std::string playerName = player->getName();
        sf::Text playerNameText(font, playerName, 24);
        const sf::FloatRect playerNameBounds = playerNameText.getLocalBounds();
        playerNameText.setOrigin({
            playerNameBounds.position.x + playerNameBounds.size.x / 2.f,
            playerNameBounds.position.y
        });
        playerNameText.setPosition({
            rect.position.x + rect.size.x / 2.f,
            rect.position.y + 12.f
        });
        playerNameText.setFillColor(accent);
        window.draw(playerNameText);

        std::string panelId;
        if (hero->getName() == "Dracula")
            panelId = "panel_dracul";
        else if (hero->getName() == "sherlock")
            panelId = "panel_sherlok";
        else if (hero->getName() == "invisible man")
            panelId = "panel_invisible";

        const float frameInset = 35.f;
        const float frameSize = rect.size.x - 2.f * frameInset;
        const float frameY = rect.position.y + 50.f;

        sf::RectangleShape portraitFrame({frameSize, frameSize});
        portraitFrame.setPosition({rect.position.x + frameInset, frameY});
        portraitFrame.setFillColor(sf::Color(18, 15, 21));
        portraitFrame.setOutlineColor(accent);
        portraitFrame.setOutlineThickness(4.f);
        window.draw(portraitFrame);

        if (!panelId.empty())
        {
            if (const sf::Texture* tex = textures.get(panelId))
            {
                sf::Sprite sprite(*tex);
                const sf::Vector2u size = tex->getSize();

                if (size.x > 0 && size.y > 0)
                {
                    const float side = static_cast<float>(std::min(size.x, size.y));
                    const float left = (static_cast<float>(size.x) - side) / 2.f;
                    const float top = (static_cast<float>(size.y) - side) / 2.f;

                    sprite.setTextureRect(sf::IntRect(
                        {static_cast<int>(left), static_cast<int>(top)},
                        {static_cast<int>(side), static_cast<int>(side)}
                    ));

                    const float imageInset = 4.f;
                    const float imageSize = frameSize - 2.f * imageInset;
                    const float scale = imageSize / side;
                    sprite.setScale({scale, scale});
                    sprite.setPosition({
                        portraitFrame.getPosition().x + imageInset,
                        portraitFrame.getPosition().y + imageInset
                    });
                    window.draw(sprite);
                }
            }
        }

        const float contentX = rect.position.x + 18.f;
        const float infoY = frameY + frameSize + 12.f;

        ui->drawText(window, hero->getName(),
                     {contentX, infoY}, 16, PARCHMENT);

        const std::string hp = "HP  " + std::to_string(hero->getHp()) +
                               "/" + std::to_string(hero->getMaxhp());
        ui->drawText(window, hp, {contentX, infoY + 28.f}, 13, PARCHMENT);

        ui->drawHealth(window, {contentX, infoY + 51.f},
                       static_cast<float>(hero->getHp()) /
                           std::max(1, hero->getMaxhp()),
                       rect.size.x - 36.f, accent);

        if (player->getDeck())
        {
            ui->drawText(window,
                         "DECK " + std::to_string(player->getDeck()->getdeckSize()),
                         {contentX, infoY + 75.f}, 12, PARCHMENT);
            ui->drawText(window,
                         "HAND " + std::to_string(player->getDeck()->gethandSize()),
                         {contentX + 120.f, infoY + 75.f}, 12, PARCHMENT);
            ui->drawText(window,
                         "DISCARD " + std::to_string(player->getDeck()->getdiscardSize()),
                         {contentX + 220.f, infoY + 75.f}, 12, PARCHMENT);
        }

        float y = infoY + 107.f;
        for (int i = 1; i < player->getfighterCount(); ++i)
        {
            Character* c = player->getFighter(i);
            if (!c) continue;

            const std::string letter =
                c->getName() == "Dr_watson" ? "W" :
                (c->getName().find("Sister") != std::string::npos ? "S" : "I");

            ui->drawText(window, letter, {contentX, y}, 17, accent);
            ui->drawText(window,
                         c->getName() + "  " +
                         std::to_string(c->getHp()) + "/" +
                         std::to_string(c->getMaxhp()),
                         {contentX + 28.f, y + 2.f}, 11,
                         c->checkalive() ? PARCHMENT :
                                           sf::Color(100, 95, 90));
            y += 28.f;
        }
    };

    drawPlayerPanel(current, {{18.f, 73.f}, {385.f, 570.f}},
                    characterColor(current ? current->getHero() : nullptr));
    drawPlayerPanel(enemy, {{1197.f, 73.f}, {385.f, 570.f}},
                    characterColor(enemy ? enemy->getHero() : nullptr));

    {
        const sf::FloatRect activeTurnRect = {{18.f, 73.f}, {385.f, 570.f}};

        sf::RectangleShape turnHighlight({activeTurnRect.size.x + 10.f, activeTurnRect.size.y + 10.f});
        turnHighlight.setPosition({activeTurnRect.position.x - 5.f, activeTurnRect.position.y - 5.f});
        turnHighlight.setFillColor(sf::Color::Transparent);
        turnHighlight.setOutlineColor(GOLD);
        turnHighlight.setOutlineThickness(4.f);
        window.draw(turnHighlight);
    }

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

    ui->drawPanel(window, {{18.f, 657.f}, {385.f, 226.f}}, GOLD);
    ui->drawText(window, "ACTIONS", {35.f, 665.f}, 17, GOLD);
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

    ui->drawPanel(window, {{1197.f, 657.f}, {385.f, 226.f}}, GOLD);
    ui->drawText(window, "TURN / DECK", {1215.f, 665.f}, 17, GOLD);
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
        case EffectPanelKind::AiTurn:  drawAiTurnPanel();        break;
        case EffectPanelKind::None:    break;
    }
    if (saveSlotPopup)
    {
        sf::RectangleShape overlay({1600.f, 900.f});
        overlay.setFillColor(sf::Color(0, 0, 0, 170));
        window.draw(overlay);

        const sf::FloatRect panel({390.f, 250.f}, {820.f, 350.f});
        ui->drawPanel(window, panel, GOLD);

        ui->drawText(window, "SAVE GAME", {700.f, 280.f}, 28, GOLD);
        ui->drawText(window, "SELECT SAVE SLOT", {700.f, 330.f}, 12, PARCHMENT);

        const float slotX = 490.f;
        const float slotY = 390.f;
        const float slotW = 180.f;
        const float slotH = 70.f;
        const float gap = 25.f;

        for (int slot = 1; slot <= 3; ++slot)
        {
            ui->drawButton(
                window,
                {{slotX + static_cast<float>(slot - 1) * (slotW + gap), slotY},
                 {slotW, slotH}},
                std::to_string(slot), true, GOLD);
        }

        ui->drawButton(window, {{565.f, 510.f}, {470.f, 58.f}},
                   "RETURN TO GAME", true, GOLD);
    }

    if (exitConfirmPopup)
    {
        sf::RectangleShape dim({1600.f, 900.f});
        dim.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(dim);

        ui->drawPanel(window, {{500.f, 365.f}, {600.f, 245.f}}, GOLD);
        ui->drawText(window, "ARE YOU SURE YOU WANT TO EXIT?",
                     {605.f, 415.f}, 20, PARCHMENT);
        ui->drawButton(window, {{650.f, 520.f}, {130.f, 48.f}}, "YES", true, RED);
        ui->drawButton(window, {{820.f, 520.f}, {130.f, 48.f}}, "NO", true, GOLD);
    }
}

void GameWindow::drawGameOver()
{
    drawFullscreenTexture("end");
    sf::RectangleShape overlay({1600.f, 900.f});
    overlay.setFillColor(sf::Color(3, 4, 8, 70));
    window.draw(overlay);

    ui->drawText(window, "THE BATTLE IS OVER", {570.f, 170.f}, 38, GOLD);
    ui->drawPanel(window, {{390.f, 280.f}, {820.f, 490.f}}, GOLD);
    ui->drawText(window, "CONGRATULATIONS", {630.f, 330.f}, 26, PARCHMENT);
    ui->drawText(window, winnerName + "", {700.f, 385.f}, 28, GOLD);
    ui->drawText(window, "has won the battle.", {675.f, 430.f}, 16, PARCHMENT);
    ui->drawText(window, "Thank you for playing UNMATCHED.", {637.f, 475.f}, 14, sf::Color(170, 162, 150));
    ui->drawText(window, "Farewell, and until the next battle...", {637.f, 505.f}, 14, sf::Color(170, 162, 150));
    ui->drawButton(window, {{610.f, 545.f}, {380.f, 50.f}}, "RETURN TO MAIN MENU", true, GOLD);
    ui->drawButton(window, {{610.f, 610.f}, {380.f, 50.f}}, "EXIT", true, RED);

    ui->drawText(window, "created & Developed by", {700.f, 800.f}, 14, sf::Color(170, 162, 150));
    ui->drawText(window, "Mahdi Dehnavi & Mohammd Javad Kazemlo", {640.f, 822.f}, 14, sf::Color(170, 162, 150));
    
    if (exitConfirmPopup)
    {
        sf::RectangleShape dim({1600.f, 900.f});
        dim.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(dim);

        ui->drawPanel(window, {{500.f, 365.f}, {600.f, 245.f}}, GOLD);
        ui->drawText(window, "ARE YOU SURE YOU WANT TO EXIT?",
                     {605.f, 415.f}, 20, PARCHMENT);
        ui->drawButton(window, {{650.f, 520.f}, {130.f, 48.f}}, "YES", true, RED);
        ui->drawButton(window, {{820.f, 520.f}, {130.f, 48.f}}, "NO", true, GOLD);
    }

}

void GameWindow::checkGameOver()
{
    if (!controller.isGameOver()) return;
    if (screen == Screen::GameOver) return;

    winnerName = controller.getGuiWinnerName();
    combatLog = controller.getGuiCombatLog();
    resetSelections();
    screen = Screen::GameOver;
    audio.playGameOverMusic();
}

void GameWindow::refreshCombatLog()
{
    combatLog = controller.getGuiCombatLog();
}

void GameWindow::handleMainMenuClick(sf::Vector2f p)
{
    if (exitConfirmPopup)
    {
        if (sf::FloatRect({650.f, 520.f}, {130.f, 48.f}).contains(p))
        {
            window.close();
            return;
        }
        if (sf::FloatRect({820.f, 520.f}, {130.f, 48.f}).contains(p))
        {
            exitConfirmPopup = false;
            return;
        }
        return;
    }

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
        audio.playSfx(AudioManager::Sfx::Confirm);
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
    {
        if (rulesView)
            rulesView->open();
        return;
    }

    if (sf::FloatRect({575.f, 665.f}, {450.f, 60.f}).contains(p))
    {
        exitConfirmPopup = true;
        return;
    }
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
            audio.playSfx(AudioManager::Sfx::Confirm);
        }
        return;
    }

    if (stage == Controller::GuiSetupStage::CharacterSelection)
    {
        const std::vector<int> choices = controller.getGuiCharacterChoices();
        for (int i = 0; i < 3; ++i)
        {
            if (sf::FloatRect({330.f + i * 330.f, 225.f}, {280.f, 350.f}).contains(p) &&
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
        if (sf::FloatRect({520.f, 550.f}, {560.f, 62.f}).contains(p))
        {
            screen = Screen::Game;
            resetSelections();
            audio.playGameplayMusic();
            audio.playSfx(AudioManager::Sfx::Confirm);
            audio.playSfx(AudioManager::Sfx::TurnStart);
            controller.guiBeginTurn();
            showMessage("The game has started.");
        }
    }
}

void GameWindow::handleGameClick(sf::Vector2f p)
{
    if (saveSlotPopup)
    {
        if (sf::FloatRect({565.f, 510.f}, {470.f, 58.f}).contains(p))
        {
            saveSlotPopup = false;
            return;
        }

        const float slotX = 490.f;
        const float slotY = 390.f;
        const float slotW = 180.f;
        const float slotH = 70.f;
        const float gap = 25.f;

        for (int slot = 1; slot <= 3; ++slot)
        {
            const sf::FloatRect button(
                {slotX + static_cast<float>(slot - 1) * (slotW + gap), slotY},
                {slotW, slotH});

            if (!button.contains(p))
                continue;

            const std::string savePath =
                (std::filesystem::path("saves") /
                 ("slot_" + std::to_string(slot) + ".json")).string();

            if (controller.guiSaveGame(savePath))
            {
                saveSlotPopup = false;
                combatLog.clear();
            }
            else
            {
                showMessage("Could not save the game.");
            }
            return;
        }

        return;
    }

    Player* current = controller.getCurrentPlayer();
    Player* enemy = controller.getEnemyPlayer();
    Character* selected = selectedCurrentCharacter();

    if (sf::FloatRect({1472.f, 12.f}, {105.f, 42.f}).contains(p))
    {
        exitConfirmPopup = true;
        return;
    }

    if (exitConfirmPopup)
    {
        if (sf::FloatRect({650.f, 520.f}, {130.f, 48.f}).contains(p))
        {
            window.close();
            return;
        }
        if (sf::FloatRect({820.f, 520.f}, {130.f, 48.f}).contains(p))
        {
            exitConfirmPopup = false;
            return;
        }
    }
    
    if (activeEffectPanel != EffectPanelKind::None)
    {
        switch (activeEffectPanel)
        {
            case EffectPanelKind::Attack:  handleAttackEffectInput(p);  break;
            case EffectPanelKind::Scheme:  handleSchemeEffectInput(p);  break;
            case EffectPanelKind::Dracula: handleDraculaEffectInput(p); break;
            case EffectPanelKind::AiTurn:  handleAiTurnInput(p);        break;
            case EffectPanelKind::None:    break;
        }
        return;
    }

    if (sf::FloatRect({305.f, 13.f}, {42.f, 42.f}).contains(p))
    {
        audio.toggleMusicMute();
        return;
    }

    if (sf::FloatRect({1375.f, 12.f}, {90.f, 42.f}).contains(p))
    {
        if (rulesView)
            rulesView->open();
        return;
    }

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

    if (sf::FloatRect({1385.f, 815.f}, {175.f, 42.f}).contains(p))
    {
        if (current && current->getDeck() && current->getDeck()->gethandSize() > 7)
        {
            handLimitMode = true;
            showMessage("You have more than 7 cards. Select cards to discard.");
            return;
        }

        controller.guiEndTurn();
        audio.playSfx(AudioManager::Sfx::TurnEnd);
        resetSelections();
        combatLog.clear();
        controller.guiBeginTurn();
        audio.playSfx(AudioManager::Sfx::TurnStart);
        showMessage("Turn changed.");
        return;
    }

    if (sf::FloatRect({35.f, 754.f}, {160.f, 43.f}).contains(p))
    {
        saveSlotPopup = true;
        return;
    }

    if (saveSlotPopup)
    {
        const float slotX = 490.f;
        const float slotY = 390.f;
        const float slotW = 180.f;
        const float slotH = 70.f;
        const float gap = 25.f;

        for (int slot = 1; slot <= 3; ++slot)
        {
            const sf::FloatRect button(
                {slotX + static_cast<float>(slot - 1) * (slotW + gap), slotY},
                {slotW, slotH});

            if (!button.contains(p))
                continue;

            const std::string savePath =
                (std::filesystem::path("saves") /
                 ("slot_" + std::to_string(slot) + ".json")).string();

            if (!controller.guiSaveGame(savePath))
            {
                showMessage("Could not save the game.");
                return;
            }

            saveSlotPopup = false;
            combatLog.clear();
            return;
        }

        return;
    }

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
                draculaPrompt = "PASSIVE ABILITY : Sherlock: effects that attempt to disable Sherlock/Dr. Watson-related cards do not disable them.";
            else
                draculaPrompt = "PASSIVE ABILITY : Invisible Man: +1 defense on a fog space (not a card effect) and movement between fog spaces is treated as adjacency.";
        }
        else
        {
            showMessage("This fighter has no special ability.");
        }
        return;
    }

    if (sf::FloatRect({35.f, 700.f}, {78.f, 43.f}).contains(p))
    {
        if (!canAct) { showMessage("You have already used both actions."); return; }
        controller.guiEndAction();
        moveMode = true;
        attackMode = false;
        schemeMode = false;
        defenseSelectionMode = false;
        awaitingDefenseDecision = false;
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
        const Card attackPlayedCard =
            controller.getCurrentPlayer()->getDeck()->getHandcard(selectedCard);
        const Card defensePlayedCard = enemy->getDeck()->gethand()[defenseCard];

        if (controller.guiAttack(attacker, defender, selectedCard, defenseCard))
        {
            const Card& attackCard = attackPlayedCard;
            activeEffectPanel = EffectPanelKind::Attack;
            audio.playSfx(AudioManager::Sfx::Attack);
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
            awaitingDefenseDecision = false;
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
                        audio.playSfx(AudioManager::Sfx::Scheme);
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
                const Card attackPlayedCard =
            controller.getCurrentPlayer()->getDeck()->getHandcard(selectedCard);
                const Card defensePlayedCard = enemyPlayer->getDeck()->gethand()[valid.front()];

                if (controller.guiAttack(selected, occupant, selectedCard, valid.front()))
                {
                    const Card& attackCard = attackPlayedCard;
                    const Card& defenseCard = defensePlayedCard;
                    activeEffectPanel = EffectPanelKind::Attack;
                    audio.playSfx(AudioManager::Sfx::Attack);
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
                defenseSelectionMode = false;
                awaitingDefenseDecision = true;
                activeEffectPanel = EffectPanelKind::Attack;
                attackResolved = false;
                attackCardName = "COMBAT";
                const Card attackPreview = current->getDeck()->getHandcard(selectedCard);
                attackCardText = attackPreview.getName() +
                    "\n\nATTACK EFFECT: " +
                    (attackPreview.geteffect().empty() ? "None" : attackPreview.geteffect()) +
                    "\n\nDEFENSE: " +
                    (enemyPlayer ? enemyPlayer->getName() : "Defender") +
                    " may choose to play a defense card.";
                attackPrompt = (enemyPlayer ? enemyPlayer->getName() : "Defender") +
                    ": do you want to use a defense card?";
                attackChoices.clear();
                attackYesNo = true;
                attackInteger = false;
                attackInputBuffer.clear();
                showMessage("Defender: choose YES to play a defense card or NO to defend without a card.");
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
            audio.playSfx(AudioManager::Sfx::Move);
            showMessage("Movement completed.");
            checkGameOver();
            advanceTurnIfTwoActionsUsed();
        }
    }
}


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
        awaitingDefenseDecision = false;
        selectedCard = -1;
        selectedEnemy = -1;
        selectedCharacter = -1;


    }
}

void GameWindow::drawAttackEffectPanel()
{
    sf::RectangleShape overlay({1600.f, 900.f});
    overlay.setFillColor(sf::Color(2, 3, 7, 175));
    window.draw(overlay);

    const sf::FloatRect panel({300.f, 55.f}, {1000.f, 790.f});
    ui->drawPanel(window, panel, GOLD);

    if (attackResolved)
    {
        const Controller::GuiCombatResult result = controller.getGuiCombatResult();

        ui->drawText(window, "COMBAT RESULT", {660.f, 85.f}, 28, GOLD);

        ui->drawText(window, "ATTACKER CARD", {480.f, 145.f}, 15, RED);
        ui->drawText(window, "DEFENDER CARD", {975.f, 145.f}, 15, BLUE);

        cardView->drawCard(window, result.attackerCard,
                           {{460.f, 180.f}, {180.f, 250.f}}, false);
        cardView->drawCard(window, result.defenderCard,
                           {{950.f, 180.f}, {180.f, 250.f}}, false);

        ui->drawPanel(window, {{385.f, 455.f}, {830.f, 105.f}}, GOLD);
        ui->drawText(window,
                     "Final Attack: " + std::to_string(result.finalAttack),
                     {445.f, 485.f}, 18, PARCHMENT);
        ui->drawText(window,
                     "Final Defense: " + std::to_string(result.finalDefense),
                     {1005.f, 485.f}, 18, PARCHMENT);

        ui->drawText(window,
                     "Winner: " + (result.winner.empty() ? "Unknown" : result.winner),
                     {665.f, 610.f}, 22, GOLD);

        const bool canClose = controller.guiEffectCanClose();
        ui->drawButton(window, {{565.f, 715.f}, {470.f, 58.f}},
                       "RETURN TO GAME", canClose, GOLD);
        return;
    }

    ui->drawText(window, attackCardName.empty() ? "ATTACK EFFECT" : attackCardName,
                 {355.f, 85.f}, 24, GOLD);

    auto drawWrapped = [&](const std::string& text, float x, float y0,
                           float widthChars, int size, sf::Color color,
                           int maxLines)
    {
        std::string line;
        int row = 0;
        const auto flush = [&]()
        {
            if (!line.empty() && row < maxLines)
                ui->drawText(window, line,
                             {x, y0 + row * (size + 7.f)}, size, color);
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
                355.f, 130.f, 102, 14, PARCHMENT, 7);

    const std::vector<std::string> logs = controller.getGuiEffectLog();
    float y = 285.f;
    int linesUsed = 0;
    int messagesShown = 0;

    for (auto it = logs.rbegin();
         it != logs.rend() && messagesShown < 7 && linesUsed < 16; ++it)
    {
        if (it->empty()) continue;

        std::string chunk;
        int wrappedLines = 0;
        for (char c : *it)
        {
            if (c == '\n')
            {
                if (!chunk.empty())
                {
                    ui->drawText(window, chunk,
                                 {355.f, y + linesUsed * 22.f}, 14, PARCHMENT);
                    ++linesUsed;
                    chunk.clear();
                }
                continue;
            }

            chunk += c;
            if (chunk.size() >= 78 && c == ' ')
            {
                ui->drawText(window, chunk,
                             {355.f, y + linesUsed * 22.f}, 14, PARCHMENT);
                ++linesUsed;
                ++wrappedLines;
                chunk.clear();
                if (wrappedLines >= 2 || linesUsed >= 16)
                    break;
            }
        }

        if (!chunk.empty() && linesUsed < 16)
        {
            ui->drawText(window, chunk,
                         {355.f, y + linesUsed * 22.f}, 14, PARCHMENT);
            ++linesUsed;
        }

        ++messagesShown;
        if (linesUsed < 16)
            ++linesUsed; // deliberate vertical separation between messages
    }

    if (!attackPrompt.empty())
        ui->drawText(window, attackPrompt, {355.f, 665.f}, 17, GOLD);

    if (attackYesNo)
    {
        ui->drawButton(window, {{470.f, 700.f}, {220.f, 50.f}}, "YES", true, GREEN);
        ui->drawButton(window, {{730.f, 700.f}, {220.f, 50.f}}, "NO", true, RED);
    }
    else if (!attackChoices.empty())
    {
        const float startX = 390.f;
        const float startY = 700.f;
        const float bw = 88.f;
        const float bh = 34.f;
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
        ui->drawPanel(window, {{470.f, 690.f}, {500.f, 55.f}}, GOLD);
        ui->drawText(window, attackInputBuffer.empty() ? "_" : attackInputBuffer,
                     {490.f, 704.f}, 18, PARCHMENT);
        ui->drawText(window, "Type the number, then press ENTER.",
                     {470.f, 752.f}, 12, PARCHMENT);
    }
}

void GameWindow::handleAttackEffectInput(sf::Vector2f p)
{
    if (attackResolved)
    {
        if (sf::FloatRect({565.f, 715.f}, {470.f, 58.f}).contains(p) && controller.guiEffectCanClose())
        {
            activeEffectPanel = EffectPanelKind::None;
            attackResolved = false;
            attackCardName.clear();
            attackCardText.clear();
            attackPrompt.clear();
            attackChoices.clear();
            attackYesNo = false;
            attackInteger = false;
            awaitingDefenseDecision = false;
            attackInputBuffer.clear();
            controller.clearGuiEffectLog();
            checkGameOver();
            advanceTurnIfTwoActionsUsed();
        }
        return;
    }

    auto resolveWithoutDefense = [&]() -> bool
    {
        Character* attacker = selectedCurrentCharacter();
        Character* defender = controller.getCharacterAt(selectedEnemy);
        if (!attacker || !defender || !controller.getCurrentPlayer() ||
            !controller.getCurrentPlayer()->getDeck() || selectedCard < 0)
        {
            attackPrompt = "Unable to resolve combat.";
            return false;
        }

        const Card attackPlayedCard =
            controller.getCurrentPlayer()->getDeck()->getHandcard(selectedCard);
        if (!controller.guiAttack(attacker, defender, selectedCard, -1))
        {
            attackPrompt = "Unable to resolve combat without a defense card.";
            showMessage("Combat could not be started.");
            return false;
        }

        activeEffectPanel = EffectPanelKind::Attack;
        audio.playSfx(AudioManager::Sfx::Attack);
        attackResolved = false;
        attackCardName = "COMBAT";
        attackCardText = attackPlayedCard.getName() +
            "\n\nATTACK EFFECT: " +
            (attackPlayedCard.geteffect().empty() ? "None" : attackPlayedCard.geteffect()) +
            "\n\nDEFENSE: No defense card played.";
        attackPrompt = "Resolving combat without a defense card...";
        attackChoices.clear();
        attackYesNo = false;
        attackInteger = false;
        attackInputBuffer.clear();
        selectedCard = -1;
        selectedEnemy = -1;
        selectedCharacter = -1;
        showMessage("The defender chose not to use a defense card. Combat is resolving.");
        checkGameOver();
        return true;
    };

    if (attackYesNo)
    {
        // These hitboxes must match the buttons drawn in drawAttackEffectPanel().
        const bool clickedYes = sf::FloatRect({470.f, 700.f}, {220.f, 50.f}).contains(p);
        const bool clickedNo  = sf::FloatRect({730.f, 700.f}, {220.f, 50.f}).contains(p);
        if (!clickedYes && !clickedNo) return;

        if (awaitingDefenseDecision)
        {
            if (clickedYes)
            {
                Character* defender = controller.getCharacterAt(selectedEnemy);
                const std::vector<int> validDefense = controller.getGuiDefenseCards(defender);

                if (validDefense.empty())
                {
                    awaitingDefenseDecision = false;
                    attackYesNo = false;
                    defenseSelectionMode = false;
                    attackPrompt = "No valid defense card is available. Resolving without defense...";
                    resolveWithoutDefense();
                    return;
                }

                awaitingDefenseDecision = false;
                attackYesNo = false;
                attackChoices.clear();
                activeEffectPanel = EffectPanelKind::None;
                defenseSelectionMode = true;
                Player* enemyPlayer = controller.getEnemyPlayer();
                attackPrompt = (enemyPlayer ? enemyPlayer->getName() : "Defender") +
                    ": choose a defense card.";
                showMessage(enemyPlayer ? enemyPlayer->getName() + ": choose a defense card."
                                        : "Choose a defense card.");
                return;
            }

            awaitingDefenseDecision = false;
            attackYesNo = false;
            defenseSelectionMode = false;
            resolveWithoutDefense();
            return;
        }

        controller.submitGuiYesNo(clickedYes);
        return;
    }

    if (!attackChoices.empty())
    {
        // Keep the input hitboxes in the exact same positions/sizes as the
        // choice buttons drawn by drawAttackEffectPanel().
        const float startX = 390.f;
        const float startY = 700.f;
        const float bw = 88.f;
        const float bh = 34.f;
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

    const std::vector<std::string> logs = controller.getGuiEffectLog();
    float y = 300.f;
    int linesUsed = 0;
    int messagesShown = 0;

    for (auto it = logs.rbegin();
         it != logs.rend() && messagesShown < 5 && linesUsed < 7; ++it)
    {
        if (it->empty()) continue;

        std::string chunk;
        int wrappedLines = 0;
        for (char c : *it)
        {
            if (c == '\n')
            {
                if (!chunk.empty())
                {
                    ui->drawText(window, chunk,
                                 {385.f, y + linesUsed * 22.f}, 14, PARCHMENT);
                    ++linesUsed;
                    chunk.clear();
                }
                continue;
            }

            chunk += c;
            if (chunk.size() >= 78 && c == ' ')
            {
                ui->drawText(window, chunk,
                             {385.f, y + linesUsed * 22.f}, 14, PARCHMENT);
                ++linesUsed;
                ++wrappedLines;
                chunk.clear();
                if (wrappedLines >= 2 || linesUsed >= 15)
                    break;
            }
        }

        if (!chunk.empty() && linesUsed < 7)
        {
            ui->drawText(window, chunk,
                         {385.f, y + linesUsed * 22.f}, 14, PARCHMENT);
            ++linesUsed;
        }

        ++messagesShown;
        if (linesUsed < 7)
            ++linesUsed;
    }

    if (!schemePrompt.empty())
    {
        const float schemePromptY = schemeResolved ? 470.f + 30.f : 470.f;
        ui->drawText(window, schemePrompt, {385.f, schemePromptY}, 16, GOLD);
    }

    if (!schemeResolved)
    {
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
                               choiceContextLabel(schemeCardText + " " + schemePrompt,
                                                  controller.getCurrentPlayer(),
                                                  schemeChoices[i]),
                               true, GOLD);
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
        abilityBody = "At the beginning of Dracula's turn, choose one adjacent living fighter (including a Sister) to take 1 damage.";
    }
    else if (heroName == "sherlock")
    {
        abilityTitle = "SHERLOCK HOLMES ABILITY";
        abilityBody = "PASSIVE: effects that try to disable Sherlock or Dr. Watson-related cards do not disable those cards.";
    }
    else if (heroName == "invisible man")
    {
        abilityTitle = "INVISIBLE MAN ABILITY";
        abilityBody = "PASSIVE: while defending on fog, defense is +1. Invisible Man may also move directly between fog spaces.";
    }

    ui->drawText(window, abilityTitle, {385.f, 100.f}, 24, GOLD);
    ui->drawText(window, abilityBody, {385.f, 145.f}, 14, PARCHMENT);

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
                               characterChoiceLabel(controller.getCurrentPlayer(),
                                                     draculaChoices[i]),
                               true, GOLD);
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

    const bool canClose = draculaResolved && controller.guiEffectCanClose();
    ui->drawButton(window, {{565.f, 705.f}, {470.f, 58.f}},
                   "RETURN TO GAME", canClose, GOLD);
}

void GameWindow::handleDraculaEffectInput(sf::Vector2f p)
{
    if (sf::FloatRect({565.f, 705.f}, {470.f, 58.f}).contains(p))
    {
        if (draculaResolved && controller.guiEffectCanClose())
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
            advanceTurnIfTwoActionsUsed();
        }
        return;
    }

    if (draculaResolved)
        return;

    if (draculaYesNo)
    {
        if (sf::FloatRect({470.f, 520.f}, {220.f, 55.f}).contains(p))
        {
            if (controller.submitGuiYesNo(true))
            {
                draculaYesNo = false;
                draculaPrompt = "Resolving Dracula's ability...";
            }
        }
        else if (sf::FloatRect({730.f, 520.f}, {220.f, 55.f}).contains(p))
        {
            if (controller.submitGuiYesNo(false))
            {
                draculaYesNo = false;
                draculaPrompt = "Skipping Dracula's ability...";
            }
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
            const sf::FloatRect button(
                {startX + col * (bw + gap), startY + row * (bh + gap)},
                {bw, bh});

            if (button.contains(p))
            {
                if (controller.submitGuiInput(draculaChoices[i]))
                    return;
            }
        }
    }
}

void GameWindow::startAiTurnIfNeeded()
{
    if (screen != Screen::Game) return;
    if (activeEffectPanel != EffectPanelKind::None) return;
    if (handLimitMode) return;
    if (controller.guiAiTurnBusy()) return;

    Player* player = controller.getCurrentPlayer();
    if (!player || !player->isAI()) return;

    resetSelections();
    combatLog.clear();
    controller.clearGuiCombatLog();
    controller.clearGuiEffectLog();

    aiTurnPrompt.clear();
    aiTurnChoices.clear();
    aiTurnYesNo = false;
    aiTurnInteger = false;
    aiTurnInputBuffer.clear();
    aiTurnSummary = false;

    if (controller.guiStartAiTurn())
    {
        activeEffectPanel = EffectPanelKind::AiTurn;
        showMessage(player->getName() + " is taking its turn...");
    }
}

void GameWindow::updateAiTurnPanel()
{
    if (aiTurnSummary)
        return;

    std::string prompt;
    std::vector<int> choices;
    bool yesNo = false;
    bool integerInput = false;

    if (controller.getGuiInputRequest(prompt, choices, yesNo, integerInput))
    {
        aiTurnPrompt = prompt;
        aiTurnChoices = choices;
        aiTurnYesNo = yesNo;
        aiTurnInteger = integerInput;
    }
    else
    {
        aiTurnPrompt.clear();
        aiTurnChoices.clear();
        aiTurnYesNo = false;
        aiTurnInteger = false;
        aiTurnInputBuffer.clear();
    }

    if (!controller.guiAiTurnBusy())
    {
        aiTurnSummary = true;
        aiTurnPrompt.clear();
        aiTurnChoices.clear();
        aiTurnYesNo = false;
        aiTurnInteger = false;
        aiTurnInputBuffer.clear();
        refreshCombatLog();
        checkGameOver();
    }
}

void GameWindow::drawAiTurnPanel()
{
    sf::RectangleShape overlay({1600.f, 900.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 30));
    window.draw(overlay);

    const sf::FloatRect panel({425.f, 82.f}, {750.f, 330.f});

    sf::RectangleShape panelBackground(panel.size);
    panelBackground.setPosition(panel.position);
    panelBackground.setFillColor(sf::Color(10, 11, 17, 218));
    panelBackground.setOutlineColor(GOLD);
    panelBackground.setOutlineThickness(1.5f);
    window.draw(panelBackground);

    sf::RectangleShape header({panel.size.x, 46.f});
    header.setPosition(panel.position);
    header.setFillColor(sf::Color(16, 16, 23, 235));
    header.setOutlineColor(sf::Color(75, 62, 45));
    header.setOutlineThickness(1.f);
    window.draw(header);

    Player* player = controller.getCurrentPlayer();
    const std::string title = player ? player->getName() + " - AI TURN" : "AI TURN";
    ui->drawText(window, title, {panel.position.x + 22.f, panel.position.y + 12.f},
                 20, GOLD);

    const std::vector<std::string> logs = controller.getGuiEffectLog();

    std::vector<std::string> visibleLogs;
    for (auto it = logs.rbegin(); it != logs.rend() && visibleLogs.size() < 6; ++it)
    {
        if (it->empty()) continue;

        std::string clean;
        for (unsigned char c : *it)
        {
            if (c >= 32 && c <= 126)
                clean += static_cast<char>(c);
        }

        if (!clean.empty())
            visibleLogs.push_back(clean);
    }

    std::reverse(visibleLogs.begin(), visibleLogs.end());

    float y = panel.position.y + 64.f;
    for (const std::string& line : visibleLogs)
    {
        std::string shown = line;
        if (shown.size() > 92)
            shown = shown.substr(0, 89) + "...";

        ui->drawText(window, shown,
                     {panel.position.x + 22.f, y},
                     12, PARCHMENT);
        y += 27.f;
    }

    if (aiTurnSummary)
    {
        ui->drawText(window, "AI TURN COMPLETE", 
                     {panel.position.x + 22.f, panel.position.y + 232.f},
                     15, GOLD);
        ui->drawText(window, "Review the final actions, then press END TURN.",
                     {panel.position.x + 22.f, panel.position.y + 258.f},
                     11, PARCHMENT);

        ui->drawButton(window,
                       {{panel.position.x + 505.f, panel.position.y + 248.f},
                        {195.f, 52.f}},
                       "END TURN", true, GOLD);
        return;
    }

    if (!aiTurnPrompt.empty())
    {
        std::string prompt = aiTurnPrompt;
        if (prompt.size() > 90)
            prompt = prompt.substr(0, 87) + "...";

        ui->drawText(window, prompt,
                     {panel.position.x + 22.f, panel.position.y + 232.f},
                     13, GOLD);

        if (aiTurnYesNo)
        {
            ui->drawButton(window,
                           {{panel.position.x + 300.f, panel.position.y + 270.f},
                            {150.f, 42.f}},
                           "YES", true, GREEN);
            ui->drawButton(window,
                           {{panel.position.x + 465.f, panel.position.y + 270.f},
                            {150.f, 42.f}},
                           "NO", true, RED);
        }
        else if (!aiTurnChoices.empty())
        {
            const float startX = panel.position.x + 22.f;
            const float startY = panel.position.y + 266.f;
            const float bw = 70.f;
            const float bh = 34.f;
            const float gap = 6.f;

            for (std::size_t i = 0; i < aiTurnChoices.size() && i < 16; ++i)
            {
                const int col = static_cast<int>(i % 8);
                const int row = static_cast<int>(i / 8);
                ui->drawButton(window,
                               {{startX + col * (bw + gap),
                                 startY + row * (bh + gap)},
                                {bw, bh}},
                               choiceContextLabel(
                                   controller.getGuiAiDecisionContext(),
                                   controller.getCurrentPlayer(),
                                   aiTurnChoices[i]),
                               true, GOLD);
            }
        }
        else if (aiTurnInteger)
        {
            ui->drawPanel(window,
                          {{panel.position.x + 250.f, panel.position.y + 266.f},
                           {250.f, 42.f}},
                          GOLD);
            ui->drawText(window,
                         aiTurnInputBuffer.empty() ? "_" : aiTurnInputBuffer,
                         {panel.position.x + 270.f, panel.position.y + 275.f},
                         16, PARCHMENT);
        }
    }
    else
    {
        ui->drawText(window, "AI is deciding...",
                     {panel.position.x + 22.f, panel.position.y + 232.f},
                     13, PARCHMENT);
    }
}

void GameWindow::handleAiTurnInput(sf::Vector2f p)
{
    const sf::FloatRect panel({425.f, 82.f}, {750.f, 330.f});

    if (aiTurnSummary)
    {
        const sf::FloatRect endButton(
            {panel.position.x + 505.f, panel.position.y + 248.f},
            {195.f, 52.f});

        if (endButton.contains(p))
        {
            Player* current = controller.getCurrentPlayer();

            if (current && current->getDeck() &&
                current->getDeck()->gethandSize() > 7)
            {
                showMessage("AI must finish its hand limit before ending the turn.");
                return;
            }

            controller.guiEndTurn();
            audio.playSfx(AudioManager::Sfx::TurnEnd);

            aiTurnSummary = false;
            activeEffectPanel = EffectPanelKind::None;
            resetSelections();
            combatLog.clear();

            controller.guiBeginTurn();
            audio.playSfx(AudioManager::Sfx::TurnStart);
            showMessage("Turn changed.");
        }
        return;
    }

    if (aiTurnYesNo)
    {
        const sf::FloatRect yesButton(
            {panel.position.x + 300.f, panel.position.y + 270.f},
            {150.f, 42.f});
        const sf::FloatRect noButton(
            {panel.position.x + 465.f, panel.position.y + 270.f},
            {150.f, 42.f});

        if (yesButton.contains(p))
            controller.submitGuiYesNo(true);
        else if (noButton.contains(p))
            controller.submitGuiYesNo(false);
        return;
    }

    if (!aiTurnChoices.empty())
    {
        const float startX = panel.position.x + 22.f;
        const float startY = panel.position.y + 266.f;
        const float bw = 70.f;
        const float bh = 34.f;
        const float gap = 6.f;

        for (std::size_t i = 0; i < aiTurnChoices.size() && i < 16; ++i)
        {
            const int col = static_cast<int>(i % 8);
            const int row = static_cast<int>(i / 8);
            const sf::FloatRect button(
                {startX + col * (bw + gap),
                 startY + row * (bh + gap)},
                {bw, bh});

            if (button.contains(p))
            {
                controller.submitGuiInput(aiTurnChoices[i]);
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
    audio.playSfx(AudioManager::Sfx::TurnEnd);
    resetSelections();
    combatLog.clear();
    controller.guiBeginTurn();
    audio.playSfx(AudioManager::Sfx::TurnStart);
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
    awaitingDefenseDecision = false;
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
    aiTurnSummary = false;
}

Character* GameWindow::selectedCurrentCharacter() const
{
    Player* current = controller.getCurrentPlayer();
    if (!current || selectedCharacter < 0 || selectedCharacter >= current->getfighterCount())
        return nullptr;
    return current->getFighter(selectedCharacter);
}