#pragma once

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/event.hpp>

#include <string>
#include <vector>

#include "core/Controller.hpp"

using namespace ftxui;

class GameTUI {
private:
    Controller* controller;
    ScreenInteractive screen;
    
    enum State { MENU, PLAYING, COMBAT, HELP, GAMEOVER };
    State state;
    bool running;
    
    int menu_select;
    int action_select;
    int card_select;
    
    Player* current;
    Player* enemy;
    std::vector<std::string> log;
    std::string message;
    
    Element renderMenu();
    Element renderGame();
    Element renderBoard();
    Element renderPlayer(Player* p, bool isCurrent);
    Element renderHand(Player* p);
    Element renderCard(const Card& c, int idx, bool sel);
    Element renderActions();
    Element renderStatus();
    Element renderLog();
    Element renderHelp();
    Element renderGameOver();
    Element renderCombat();
    
    std::string getCharSymbol(Character* c);
    void updateData();
    
public:
    GameTUI(Controller* ctrl);
    ~GameTUI() = default;
    
    void run();
    void refresh();
    void setState(State s);
    void showMessage(const std::string& msg);
    void addLog(const std::string& msg);
};