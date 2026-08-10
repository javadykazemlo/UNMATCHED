#pragma once
#include <SFML/Graphics.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <string>
#include <vector>
#include <utility>

#include "core/Controller.hpp"
#include "core/Player.hpp"
#include "graphics/UI.hpp"
#include "graphics/BoardView.hpp"
#include "graphics/DeckView.hpp"
#include "graphics/CharacterView.hpp"

// ─────────────────────────────────────────────────────────────────────────
//  GameWindow
//
//  This is the only file that talks to Controller. Controller (and every
//  class under core/, cards/, entities/, Save/) was written as a console
//  program: choosePlayers(), chooseCharacters(), playTurn(), move(),
//  Scheme(), startCombat()... all read blocking input through the public
//  getInt()/getChoice()/getYesNo() methods, which read straight from
//  std::cin, and all of them narrate what they are doing through std::cout.
//
//  Rather than reimplementing (and risking desynchronising) the game
//  rules, GameWindow runs the *unmodified* Controller on a background
//  thread and:
//    1. temporarily redirects std::cin to an in-memory queue that GUI
//       button clicks push lines into (InputBridge),
//    2. temporarily redirects std::cout to an in-memory buffer so the
//       narration Controller already produces ("Choose a action:",
//       "Available spaces: 4 5 6", "Selected card:", ...) can be read
//       and turned into a themed panel with real buttons instead of a
//       terminal prompt.
//  The background thread is only ever touching game state while it is
//  actively running; the render thread only reads game state at the
//  instant the background thread reports it has parked waiting for the
//  next answer, which keeps the two threads out of each other's way.
//
//  main.cpp only calls isOpen() / pollAndHandleEvents() / clear() /
//  display(), so that is exactly the public surface kept here.
// ─────────────────────────────────────────────────────────────────────────

// Feeds queued lines of text to std::cin, one line per GUI answer.
class InputBridge : public std::streambuf
{
public:
    void pushLine(const std::string& line);
    bool waitingForInput() const { return waiting_.load(); }
    std::uint64_t generation() const { return generation_.load(); }

protected:
    int_type underflow() override;

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::string> queue_;
    std::string current_;
    std::atomic<bool> waiting_{false};
    std::atomic<std::uint64_t> generation_{0};
};

// Captures everything Controller prints through std::cout so it can be
// shown inside a themed panel instead of a terminal.
class OutputCapture : public std::streambuf
{
public:
    std::string consume();

protected:
    int_type overflow(int_type ch) override;

private:
    std::mutex mutex_;
    std::string buffer_;
};

class GameWindow
{
public:
    GameWindow();
    ~GameWindow();

    bool isOpen() const;
    void pollAndHandleEvents();
    void clear();
    void display();

private:
    // What kind of prompt Controller is currently blocked on, guessed from
    // the text it just printed. Purely a presentation hint: whichever
    // button the player clicks is validated by the real Controller, never
    // by this classification.
    enum class PromptKind
    {
        None, MainMenu, SaveSlot, Age, Name, YesNo, CharacterSelect,
        PositionSelect, BoardPick, NumberedChoice, CardPick, NumberGuess, Generic
    };

    sf::RenderWindow window_;

    // engine + bridge --------------------------------------------------------
    Controller controller_;
    Player players_[2];
    std::thread engineThread_;
    InputBridge inputBridge_;
    OutputCapture outputCapture_;
    std::streambuf* savedCinBuf_ = nullptr;
    std::streambuf* savedCoutBuf_ = nullptr;
    std::atomic<bool> engineFinished_{false};
    std::string engineError_;
    std::uint64_t lastSeenGeneration_ = 0;

    // current prompt state (rebuilt only at safe points) --------------------
    std::string promptText_;
    PromptKind promptKind_ = PromptKind::None;
    std::vector<std::pair<int, std::string>> promptOptions_;
    std::vector<int> promptInts_;
    std::string promptTitle_;

    // persistent scene views --------------------------------------------------
    BoardView boardView_;
    DeckView handView_;
    Panel player1Panel_, player2Panel_, actionsPanelBg_, promptPanelBg_, logPanelBg_;

    // dynamic widgets for whichever prompt is active -------------------------
    std::vector<Button> promptButtons_;
    SpinBox ageSpin_{ {0,0}, {160.f, 50.f} };
    TextField nameField_{ {0,0}, {320.f, 46.f} };
    Button okButton_{ "OK", {0,0}, {140.f, 46.f} };
    Button retreatButton_{ "RETREAT", {0,0}, {140.f, 46.f} };

    std::string recentLog_;

    void buildStaticLayout();
    void pumpEngineOutput();
    void classifyPrompt();
    void rebuildPromptWidgets();

    void handleMouseMoved(sf::Vector2f pos);
    void handleMousePressed(sf::Vector2f pos);
    void handleTextEntered(std::uint32_t unicode);
    void handleKeyPressed(const sf::Event::KeyPressed& key);

    void drawBackdrop();
    void drawSceneIfStarted();
    void drawPromptPanel();
    void drawGameOverIfNeeded();

    void send(const std::string& line);
    void sendInt(int value);

    void runEngine();
};