#include "graphics/GameWindow.hpp"
#include "graphics/CardView.hpp"
#include "graphics/TextureManager.hpp"
#include <cctype>
#include <algorithm>
#include <sstream>

// ─────────────────────────────────────────────────────────────────────────
//  InputBridge / OutputCapture
// ─────────────────────────────────────────────────────────────────────────
void InputBridge::pushLine(const std::string& line)
{
    std::lock_guard<std::mutex> lk(mutex_);
    queue_.push(line + "\n");
    cv_.notify_one();
}

InputBridge::int_type InputBridge::underflow()
{
    if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());

    std::unique_lock<std::mutex> lk(mutex_);
    waiting_.store(true);
    generation_.fetch_add(1);
    cv_.wait(lk, [this] { return !queue_.empty(); });
    waiting_.store(false);

    current_ = queue_.front();
    queue_.pop();
    char* begin = &current_[0];
    setg(begin, begin, begin + current_.size());
    return traits_type::to_int_type(*begin);
}

OutputCapture::int_type OutputCapture::overflow(int_type ch)
{
    if (ch != traits_type::eof())
    {
        std::lock_guard<std::mutex> lk(mutex_);
        buffer_ += static_cast<char>(ch);
    }
    return ch;
}

std::string OutputCapture::consume()
{
    std::lock_guard<std::mutex> lk(mutex_);
    std::string out = std::move(buffer_);
    buffer_.clear();
    return out;
}

// ─────────────────────────────────────────────────────────────────────────
//  small text-parsing helpers (purely cosmetic: they only decide which
//  buttons to draw, the real validation always happens inside Controller)
// ─────────────────────────────────────────────────────────────────────────
namespace
{
    bool has(const std::string& hay, const std::string& needle)
    {
        return hay.find(needle) != std::string::npos;
    }

    std::vector<int> intsAfter(const std::string& text, const std::string& label)
    {
        std::vector<int> out;
        std::size_t pos = text.rfind(label);
        if (pos == std::string::npos) return out;
        pos += label.size();
        std::size_t end = text.find('\n', pos);
        std::string seg = (end == std::string::npos) ? text.substr(pos) : text.substr(pos, end - pos);

        std::size_t i = 0;
        while (i < seg.size())
        {
            if (std::isdigit(static_cast<unsigned char>(seg[i])))
            {
                std::size_t j = i;
                while (j < seg.size() && std::isdigit(static_cast<unsigned char>(seg[j]))) ++j;
                try { out.push_back(std::stoi(seg.substr(i, j - i))); } catch (...) {}
                i = j;
            }
            else ++i;
        }
        return out;
    }

    std::vector<std::pair<int, std::string>> numberedList(const std::string& text)
    {
        std::vector<std::pair<int, std::string>> out;
        std::size_t i = 0;
        while (i < text.size())
        {
            std::size_t lineEnd = text.find('\n', i);
            std::string line = (lineEnd == std::string::npos) ? text.substr(i) : text.substr(i, lineEnd - i);

            std::size_t s = line.find_first_not_of(" \t");
            if (s != std::string::npos)
            {
                std::size_t d = s;
                while (d < line.size() && std::isdigit(static_cast<unsigned char>(line[d]))) ++d;
                if (d > s && d < line.size() && line[d] == '.')
                {
                    try
                    {
                        int num = std::stoi(line.substr(s, d - s));
                        std::size_t nameStart = d + 1;
                        while (nameStart < line.size() && line[nameStart] == ' ') ++nameStart;
                        std::string name = line.substr(nameStart);
                        while (!name.empty() && (name.back() == '\r' || name.back() == ' ')) name.pop_back();
                        if (!name.empty()) out.emplace_back(num, name);
                    }
                    catch (...) {}
                }
            }
            if (lineEnd == std::string::npos) break;
            i = lineEnd + 1;
        }
        return out;
    }
}

// ─────────────────────────────────────────────────────────────────────────
//  GameWindow
// ─────────────────────────────────────────────────────────────────────────
GameWindow::GameWindow()
    : window_(sf::VideoMode({1600u, 900u}), "UNMATCHED - Cobble & Fog"),
      boardView_({40.f, 210.f}, {900.f, 560.f}),
      handView_({40.f, 700.f}, {1520.f, 190.f}),
      player1Panel_({960.f, 20.f}, {300.f, 180.f}, sf::Color(35, 16, 20, 235), Theme::Player1),
      player2Panel_({1270.f, 20.f}, {300.f, 180.f}, sf::Color(16, 24, 38, 235), Theme::Player2),
      actionsPanelBg_({960.f, 210.f}, {610.f, 300.f}),
      promptPanelBg_({420.f, 260.f}, {760.f, 420.f}),
      logPanelBg_({960.f, 520.f}, {610.f, 170.f})
{
    window_.setFramerateLimit(60);

    sf::View view(sf::Vector2f(800.f, 450.f), sf::Vector2f(1600.f, 900.f));
    window_.setView(view);

    buildStaticLayout();

    savedCoutBuf_ = std::cout.rdbuf(&outputCapture_);
    savedCinBuf_ = std::cin.rdbuf(&inputBridge_);

    engineThread_ = std::thread(&GameWindow::runEngine, this);
}

GameWindow::~GameWindow()
{
    // The engine thread may be permanently parked inside std::cin waiting
    // for an answer nobody will ever send once the window is closed; it is
    // not safe to join it, so let it die with the process instead.
    if (engineThread_.joinable())
        engineThread_.detach();

    std::cin.rdbuf(savedCinBuf_);
    std::cout.rdbuf(savedCoutBuf_);
}

void GameWindow::runEngine()
{
    try
    {
        controller_.startMenu(players_);
        while (!controller_.end_game())
            controller_.playTurn();
    }
    catch (const std::exception& e)
    {
        engineError_ = e.what();
    }
    catch (...)
    {
        engineError_ = "The game engine stopped unexpectedly.";
    }
    engineFinished_.store(true);
}

void GameWindow::buildStaticLayout()
{
    actionsPanelBg_.setTitle("ACTIONS");
    logPanelBg_.setTitle("GAME LOG");
    player1Panel_.setTitle("PLAYER 1");
    player2Panel_.setTitle("PLAYER 2");
    promptPanelBg_.setTitle("");
}

bool GameWindow::isOpen() const
{
    return window_.isOpen();
}

void GameWindow::send(const std::string& line)
{
    inputBridge_.pushLine(line);
}

void GameWindow::sendInt(int value)
{
    send(std::to_string(value));
}

// ── per-frame event handling ────────────────────────────────────────────
void GameWindow::pollAndHandleEvents()
{
    pumpEngineOutput();

    while (const std::optional<sf::Event> event = window_.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window_.close();
        }
        else if (const auto* resized = event->getIf<sf::Event::Resized>())
        {
            (void)resized;
            sf::View view(sf::Vector2f(800.f, 450.f), sf::Vector2f(1600.f, 900.f));
            window_.setView(view);
        }
        else if (const auto* moved = event->getIf<sf::Event::MouseMoved>())
        {
            sf::Vector2f world = window_.mapPixelToCoords(moved->position);
            handleMouseMoved(world);
        }
        else if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (pressed->button == sf::Mouse::Button::Left)
            {
                sf::Vector2f world = window_.mapPixelToCoords(pressed->position);
                handleMousePressed(world);
            }
        }
        else if (const auto* text = event->getIf<sf::Event::TextEntered>())
        {
            handleTextEntered(text->unicode);
        }
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            handleKeyPressed(*key);
        }
    }
}

void GameWindow::pumpEngineOutput()
{
    if (inputBridge_.waitingForInput() && inputBridge_.generation() != lastSeenGeneration_)
    {
        lastSeenGeneration_ = inputBridge_.generation();
        promptText_ = outputCapture_.consume(); // only this prompt's own text
        recentLog_ += promptText_;
        if (recentLog_.size() > 4000) recentLog_ = recentLog_.substr(recentLog_.size() - 4000);
        classifyPrompt();
        rebuildPromptWidgets();
    }
}

// ── classification ──────────────────────────────────────────────────────
void GameWindow::classifyPrompt()
{
    // Classify using ONLY the text produced since the previous prompt
    // (promptText_), never the cumulative recentLog_. Otherwise a short
    // keyword from an earlier screen (e.g. the very first "1. New Game")
    // would still be inside a trailing window many prompts later and keep
    // re-triggering the wrong screen forever.
    const std::string& tail = promptText_;

    if (has(tail, "1. New Game") && has(tail, "2. Load Game"))
    { promptKind_ = PromptKind::MainMenu; promptTitle_ = "UNMATCHED"; return; }

    if (has(tail, "save slot"))
    { promptKind_ = PromptKind::SaveSlot; promptTitle_ = "CHOOSE A SAVE SLOT"; return; }

    if (has(tail, "enter your age"))
    { promptKind_ = PromptKind::Age; promptTitle_ = "ASSESSMENT OF MORTALITY"; return; }

    if (has(tail, "enter your name"))
    { promptKind_ = PromptKind::Name; promptTitle_ = "DECLARE YOUR NAME"; return; }

    if (has(tail, "(y/n)"))
    { promptKind_ = PromptKind::YesNo; promptTitle_ = "A DECISION IS REQUIRED"; return; }

    if (has(tail, "choose your Character"))
    { promptKind_ = PromptKind::CharacterSelect; promptTitle_ = "CHOOSE YOUR LEGEND"; return; }

    if (has(tail, "left map"))
    { promptKind_ = PromptKind::PositionSelect; promptTitle_ = "CHOOSE YOUR GROUND"; return; }

    if (has(tail, "where do you want to place") || has(tail, "Select a destination"))
    { promptKind_ = PromptKind::BoardPick; promptTitle_ = "CHOOSE A SPACE ON THE MAP"; return; }

    if (has(tail, "guess the attack value"))
    { promptKind_ = PromptKind::NumberGuess; promptTitle_ = "ELEMENTARY, MY DEAR..."; return; }

    if (has(tail, "Choose a card") || has(tail, "Selected card:") || has(tail, "card number to remove"))
    { promptKind_ = PromptKind::CardPick; promptTitle_ = "CHOOSE A CARD"; return; }

    if (has(tail, "Choose a"))
    { promptKind_ = PromptKind::NumberedChoice; promptTitle_ = "MAKE YOUR CHOICE"; return; }

    promptKind_ = PromptKind::Generic;
    promptTitle_ = "THE STORY CONTINUES";
}

void GameWindow::rebuildPromptWidgets()
{
    promptButtons_.clear();
    promptOptions_.clear();
    promptInts_.clear();

    const std::string& t = promptText_;
    sf::Vector2f base = { 460.f, 340.f };
    float bw = 300.f, bh = 54.f, gap = 14.f;

    switch (promptKind_)
    {
        case PromptKind::MainMenu:
            promptButtons_.emplace_back("\u2694  START", sf::Vector2f{base.x, base.y}, sf::Vector2f{bw, bh}, sf::Color(120, 20, 20));
            promptButtons_.emplace_back("\U0001F4D6  LOAD GAME", sf::Vector2f{base.x, base.y + bh + gap}, sf::Vector2f{bw, bh});
            promptButtons_.emplace_back("EXIT", sf::Vector2f{base.x, base.y + 2 * (bh + gap)}, sf::Vector2f{bw, bh});
            break;

        case PromptKind::SaveSlot:
            for (int i = 1; i <= 3; ++i)
                promptButtons_.emplace_back("SLOT " + std::to_string(i),
                    sf::Vector2f{base.x, base.y + (i - 1) * (bh + gap)}, sf::Vector2f{bw, bh});
            break;

        case PromptKind::Age:
            ageSpin_.setPosition({ base.x, base.y });
            ageSpin_.setValue(25);
            okButton_.setPosition({ base.x + 20.f, base.y + 90.f });
            break;

        case PromptKind::Name:
            nameField_.setPosition({ base.x, base.y });
            nameField_.setValue("");
            nameField_.setFocused(true);
            okButton_.setPosition({ base.x + 90.f, base.y + 70.f });
            break;

        case PromptKind::YesNo:
            promptButtons_.emplace_back("YES", sf::Vector2f{base.x, base.y}, sf::Vector2f{140.f, bh}, Theme::Success);
            promptButtons_.emplace_back("NO", sf::Vector2f{base.x + 160.f, base.y}, sf::Vector2f{140.f, bh}, Theme::Danger);
            break;

        case PromptKind::CharacterSelect:
        {
            std::vector<std::string> names = { "Dracula", "Sherlock", "Invisible Man" };
            for (int i = 0; i < 3; ++i)
                promptButtons_.emplace_back(names[i], sf::Vector2f{base.x, base.y + i * (bh + gap)}, sf::Vector2f{bw, bh});
            break;
        }

        case PromptKind::PositionSelect:
            promptButtons_.emplace_back("LEFT SIDE OF THE MAP", sf::Vector2f{base.x, base.y}, sf::Vector2f{bw, bh});
            promptButtons_.emplace_back("RIGHT SIDE OF THE MAP", sf::Vector2f{base.x, base.y + bh + gap}, sf::Vector2f{bw, bh});
            break;

        case PromptKind::BoardPick:
            promptInts_ = intsAfter(t, "Available space:");
            if (promptInts_.empty()) promptInts_ = intsAfter(t, "Available spaces:");
            boardView_.setHighlighted(promptInts_);
            break;

        case PromptKind::NumberGuess:
            ageSpin_.setPosition({ base.x, base.y });
            ageSpin_.setValue(2);
            okButton_.setPosition({ base.x + 20.f, base.y + 90.f });
            break;

        case PromptKind::CardPick:
        {
            promptInts_ = intsAfter(t, "Available cards:");
            handView_.setPlayableIndices(promptInts_);
            handView_.setExpanded(true);
            break;
        }

        case PromptKind::NumberedChoice:
        {
            promptOptions_ = numberedList(t);
            int row = 0;
            for (auto& opt : promptOptions_)
            {
                std::string label = std::to_string(opt.first) + ".  " + opt.second;
                promptButtons_.emplace_back(label, sf::Vector2f{base.x, base.y + row * (bh + gap)}, sf::Vector2f{bw, bh});
                ++row;
            }
            break;
        }

        case PromptKind::Generic:
        default:
            okButton_.setPosition({ base.x + 80.f, base.y + 120.f });
            break;
    }
}

// ── input handling ──────────────────────────────────────────────────────
void GameWindow::handleMouseMoved(sf::Vector2f pos)
{
    for (auto& b : promptButtons_) b.updateHover(pos);
    okButton_.updateHover(pos);
    retreatButton_.updateHover(pos);
    ageSpin_.updateHover(pos);
    boardView_.updateHover(pos);
    handView_.updateHover(pos);
}

void GameWindow::handleMousePressed(sf::Vector2f pos)
{
    nameField_.handleClick(pos);

    switch (promptKind_)
    {
        case PromptKind::MainMenu:
            if (promptButtons_.size() == 3)
            {
                if (promptButtons_[0].click(pos)) { send("1"); return; }
                if (promptButtons_[1].click(pos)) { send("2"); return; }
                if (promptButtons_[2].click(pos)) { window_.close(); return; }
            }
            break;

        case PromptKind::SaveSlot:
            for (std::size_t i = 0; i < promptButtons_.size(); ++i)
                if (promptButtons_[i].click(pos)) { sendInt(static_cast<int>(i) + 1); return; }
            break;

        case PromptKind::Age:
            if (ageSpin_.click(pos)) return;
            if (okButton_.click(pos)) { sendInt(ageSpin_.getValue()); return; }
            break;

        case PromptKind::NumberGuess:
            if (ageSpin_.click(pos)) return;
            if (okButton_.click(pos)) { sendInt(ageSpin_.getValue()); return; }
            break;

        case PromptKind::Name:
            if (okButton_.click(pos))
            {
                std::string v = nameField_.getValue();
                if (v.empty()) v = "Player";
                send(v);
                return;
            }
            break;

        case PromptKind::YesNo:
            if (promptButtons_.size() == 2)
            {
                if (promptButtons_[0].click(pos)) { send("y"); return; }
                if (promptButtons_[1].click(pos)) { send("n"); return; }
            }
            break;

        case PromptKind::CharacterSelect:
            for (std::size_t i = 0; i < promptButtons_.size(); ++i)
                if (promptButtons_[i].click(pos)) { sendInt(static_cast<int>(i) + 1); return; }
            break;

        case PromptKind::PositionSelect:
            if (promptButtons_.size() == 2)
            {
                if (promptButtons_[0].click(pos)) { send("1"); return; }
                if (promptButtons_[1].click(pos)) { send("2"); return; }
            }
            break;

        case PromptKind::BoardPick:
        {
            int node = boardView_.click(pos);
            if (node >= 0) { sendInt(node); return; }
            break;
        }

        case PromptKind::CardPick:
        {
            int idx = handView_.click(pos);
            if (idx > 0) { sendInt(idx); return; }
            break;
        }

        case PromptKind::NumberedChoice:
            for (std::size_t i = 0; i < promptButtons_.size() && i < promptOptions_.size(); ++i)
                if (promptButtons_[i].click(pos)) { sendInt(promptOptions_[i].first); return; }
            // characters on screen are also clickable shortcuts ----------------
            {
                int boardHit = boardView_.click(pos);
                if (boardHit >= 0)
                {
                    Character* c = controller_.getBord().getCharacter(boardHit);
                    if (c)
                    {
                        for (auto& opt : promptOptions_)
                            if (opt.second == c->getName()) { sendInt(opt.first); return; }
                    }
                }
            }
            break;

        case PromptKind::Generic:
        default:
            if (okButton_.click(pos)) { sendInt(ageSpin_.getValue()); return; }
            break;
    }
}

void GameWindow::handleTextEntered(std::uint32_t unicode)
{
    if (promptKind_ == PromptKind::Name)
        nameField_.handleTextEntered(unicode);
}

void GameWindow::handleKeyPressed(const sf::Event::KeyPressed& key)
{
    if (promptKind_ == PromptKind::Name && key.code == sf::Keyboard::Key::Enter)
    {
        std::string v = nameField_.getValue();
        if (v.empty()) v = "Player";
        send(v);
    }
}

// ── drawing ──────────────────────────────────────────────────────────────
void GameWindow::clear()
{
    window_.clear(Theme::Background);

    drawBackdrop();
    drawSceneIfStarted();
    drawPromptPanel();
    drawGameOverIfNeeded();
}

void GameWindow::display()
{
    window_.display();
}

void GameWindow::drawBackdrop()
{
    sf::RectangleShape bg(sf::Vector2f(1600.f, 900.f));
    bg.setPosition({0.f, 0.f});
    bg.setFillColor(Theme::Background);
    window_.draw(bg);

    sf::Text title(Theme::titleFont(), "U N M A T C H E D", 30);
    title.setFillColor(Theme::Gold);
    Theme::centerOrigin(title);
    title.setPosition({800.f, 40.f});
    window_.draw(title);

    sf::Text subtitle(Theme::bodyFont(), "Cobble & Fog", 16);
    subtitle.setFillColor(Theme::TextMuted);
    Theme::centerOrigin(subtitle);
    subtitle.setPosition({800.f, 72.f});
    window_.draw(subtitle);
}

void GameWindow::drawSceneIfStarted()
{
    Player* current = controller_.getCurrentPlayer();
    Player* enemy = controller_.getEnemyPlayer();
    if (!current || !enemy) return; // still in menu / setup screens

    // player panels -----------------------------------------------------------
    player1Panel_.draw(window_);
    player2Panel_.draw(window_);

    Player* panelOwners[2] = { current->getHero() && current->getHero()->getowner() == 1 ? current : enemy,
                               nullptr };
    // simpler: figure owner directly from character->getowner()
    Player* p1 = (current->getHero() && current->getHero()->getowner() == 1) ? current : enemy;
    Player* p2 = (p1 == current) ? enemy : current;

    auto drawRoster = [this](Player* p, sf::Vector2f origin)
    {
        if (!p) return;
        float x = origin.x;
        float y = origin.y;
        for (Character* c : p->getCharacters())
        {
            if (!c) continue;
            CharacterView cv({ x, y }, 26.f);
            cv.setCharacter(c);
            cv.draw(window_);
            x += 70.f;
        }
        sf::Text name(Theme::bodyFont(), p->getName() + (p->isAI() ? " (AI)" : ""), 14);
        name.setFillColor(Theme::TextLight);
        name.setPosition({ origin.x - 5.f, origin.y + 60.f });
        window_.draw(name);
    };

    drawRoster(p1, { 985.f, 90.f });
    drawRoster(p2, { 1295.f, 90.f });

    // board ---------------------------------------------------------------------
    boardView_.setBoard(&controller_.getBord());
    if (promptKind_ != PromptKind::BoardPick) boardView_.setHighlighted({});
    boardView_.draw(window_);

    // hand of the player currently deciding --------------------------------
    Player* decider = current; // whichever player Controller last asked to act
    handView_.setHand(decider->getDeck()->gethand());
    handView_.setDeckCount(decider->getDeck()->getdeckSize());
    handView_.setDiscardCount(decider->getDeck()->getdiscardSize());
    if (promptKind_ != PromptKind::CardPick) handView_.clearPlayableIndices();
    handView_.draw(window_);

    // turn banner ---------------------------------------------------------------
    sf::Text turnBanner(Theme::titleFont(), current->getName() + "'s turn", 20);
    turnBanner.setFillColor(current->getHero() && current->getHero()->getowner() == 1 ? Theme::Player1 : Theme::Player2);
    Theme::centerOrigin(turnBanner);
    turnBanner.setPosition({490.f, 185.f});
    window_.draw(turnBanner);

    // log panel --------------------------------------------------------------------
    logPanelBg_.draw(window_);
    Label log(recentLog_.substr(recentLog_.size() > 900 ? recentLog_.size() - 900 : 0), 12, Theme::TextMuted);
    log.setWrapWidth(580.f);
    log.setPosition({ 972.f, 552.f });
    log.draw(window_);

    if (!inputBridge_.waitingForInput())
    {
        sf::Text thinking(Theme::bodyFont(), "The story is unfolding...", 14);
        thinking.setFillColor(Theme::Gold);
        thinking.setPosition({972.f, 200.f});
        window_.draw(thinking);
    }
}

void GameWindow::drawPromptPanel()
{
    if (!inputBridge_.waitingForInput()) return;
    if (engineFinished_.load()) return;

    bool overlayMode = (controller_.getCurrentPlayer() != nullptr && controller_.getEnemyPlayer() != nullptr);

    Panel panel = overlayMode
        ? Panel({420.f, 260.f}, {760.f, 300.f})
        : Panel({420.f, 220.f}, {760.f, 480.f});
    panel.draw(window_);

    sf::Text title(Theme::titleFont(), promptTitle_, 24);
    title.setFillColor(Theme::GoldBright);
    Theme::centerOrigin(title);
    title.setPosition({800.f, overlayMode ? 300.f : 265.f});
    window_.draw(title);

    switch (promptKind_)
    {
        case PromptKind::Age:
        {
            sf::Text lbl(Theme::bodyFont(), "Enter age:", 16);
            lbl.setFillColor(Theme::TextLight);
            lbl.setPosition({460.f, 320.f});
            window_.draw(lbl);
            ageSpin_.draw(window_);
            okButton_.draw(window_);
            break;
        }
        case PromptKind::NumberGuess:
        {
            sf::Text lbl(Theme::bodyFont(), "Guess the attack value:", 16);
            lbl.setFillColor(Theme::TextLight);
            lbl.setPosition({460.f, 320.f});
            window_.draw(lbl);
            ageSpin_.draw(window_);
            okButton_.draw(window_);
            break;
        }
        case PromptKind::Name:
            nameField_.draw(window_);
            okButton_.draw(window_);
            break;

        case PromptKind::BoardPick:
        {
            sf::Text hint(Theme::bodyFont(), "Click a glowing space on the map.", 15);
            hint.setFillColor(Theme::TextMuted);
            Theme::centerOrigin(hint);
            hint.setPosition({800.f, overlayMode ? 340.f : 330.f});
            window_.draw(hint);
            break;
        }

        case PromptKind::CardPick:
        {
            sf::Text hint(Theme::bodyFont(), "Click a card in your hand below.", 15);
            hint.setFillColor(Theme::TextMuted);
            Theme::centerOrigin(hint);
            hint.setPosition({800.f, overlayMode ? 340.f : 330.f});
            window_.draw(hint);
            break;
        }

        case PromptKind::Generic:
        {
            Label body(promptText_.empty() ? recentLog_.substr(recentLog_.size() > 400 ? recentLog_.size() - 400 : 0) : promptText_,
                       14, Theme::TextLight);
            body.setWrapWidth(680.f);
            body.setPosition({460.f, 320.f});
            body.draw(window_);

            sf::Text lbl(Theme::bodyFont(), "Type a number and press OK:", 14);
            lbl.setFillColor(Theme::TextMuted);
            lbl.setPosition({460.f, overlayMode ? 400.f : 470.f});
            window_.draw(lbl);
            ageSpin_.setPosition({460.f, overlayMode ? 425.f : 495.f});
            ageSpin_.draw(window_);
            okButton_.draw(window_);
            break;
        }

        default:
            break;
    }

    for (auto& b : promptButtons_) b.draw(window_);
}

void GameWindow::drawGameOverIfNeeded()
{
    if (!engineFinished_.load()) return;

    sf::RectangleShape overlay(sf::Vector2f(1600.f, 900.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 210));
    window_.draw(overlay);

    sf::Text title(Theme::titleFont(), "THE TALE HAS ENDED", 40);
    title.setFillColor(Theme::GoldBright);
    Theme::centerOrigin(title);
    title.setPosition({800.f, 380.f});
    window_.draw(title);

    std::string msg = engineError_.empty() ? "Thank you for playing." : engineError_;
    sf::Text sub(Theme::bodyFont(), msg, 18);
    sub.setFillColor(Theme::TextLight);
    Theme::centerOrigin(sub);
    sub.setPosition({800.f, 440.f});
    window_.draw(sub);
}