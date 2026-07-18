#pragma once

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/event.hpp>

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "core/Controller.hpp"

using namespace ftxui;

class GameTUI {
private:
    Controller* controller;
    ScreenInteractive screen;
    
    enum State { MENU, PLAYING, COMBAT, HELP, GAMEOVER, CARD_SELECT };
    State state;
    bool running;
    
    int menu_select;
    int action_select;
    int card_select;
    int combat_card_select;
    int move_target_select;
    
    Player* current;
    Player* enemy;
    std::vector<std::string> log;
    std::string message;
    std::string input_buffer;
    bool input_mode;
    
    std::vector<std::string> menu_items = {"Play Game", "Help", "Exit"};
    std::vector<std::string> action_items = {"Attack", "Maneuver", "Scheme", "End Turn", "Help"};
    
    Element renderMenu();
    Element renderGame();
    Element renderBoard();
    Element renderPlayer(Player* p, bool isCurrent);
    Element renderHand(Player* p, int selected = -1);
    Element renderCard(const Card& c, int idx, bool sel);
    Element renderActions();
    Element renderStatus();
    Element renderLog();
    Element renderHelp();
    Element renderGameOver();
    Element renderCombat();
    Element renderCardSelect();
    
    std::string getCharSymbol(Character* c);
    std::string getHPBar(int current, int max);
    std::string centerText(const std::string& text, int width);
    void updateData();
    void processCommand(const std::string& cmd);
    bool handleKeyPress(Event event);
    
public:
    GameTUI(Controller* ctrl);
    ~GameTUI() = default;
    
    void run();
    void refresh();
    void setState(State s);
    void showMessage(const std::string& msg);
    void addLog(const std::string& msg);
    void setInputMode(bool mode) { input_mode = mode; }
};