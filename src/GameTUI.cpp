#include "GameTUI.hpp"
#include <iostream>

using namespace ftxui;

GameTUI::GameTUI(Controller* ctrl) 
    : controller(ctrl),
      screen(ScreenInteractive::Fullscreen()),
      state(MENU),
      running(true),
      menu_select(0),
      action_select(0),
      card_select(-1),
      current(nullptr),
      enemy(nullptr) {
    
    log.push_back("Welcome to Unmatched!");
    log.push_back("Dracula vs Sherlock Holmes");
    log.push_back("Select your hero and begin the battle!");
    updateData();
}

void GameTUI::run() {
    while (running) {
        Element screen_element;
        
        switch (state) {
            case MENU: screen_element = renderMenu(); break;
            case PLAYING: screen_element = renderGame(); break;
            case COMBAT: screen_element = renderCombat(); break;
            case HELP: screen_element = renderHelp(); break;
            case GAMEOVER: screen_element = renderGameOver(); break;
        }
        
        screen.Loop(Renderer([&] { return screen_element; }));
    }
}

Element GameTUI::renderMenu() {
    std::vector<std::string> items = {"Play", "Help", "Exit"};
    std::vector<Element> elems;
    
    for (int i = 0; i < items.size(); i++) {
        auto e = text(items[i]);
        if (i == menu_select) {
            e = e | bold | color(Color::Green) | border;
        }
        elems.push_back(e | size(WIDTH, EQUAL, 30));
    }
    
    std::vector<Element> menu_elems;
    menu_elems.push_back(text("UNMATCHED: THE GAME") | bold | color(Color::Red));
    menu_elems.push_back(text("Dracula vs Sherlock Holmes") | color(Color::Yellow));
    menu_elems.push_back(vbox(elems) | center);
    menu_elems.push_back(text("↑/↓: Navigate | ENTER: Select | Q: Quit") | dim);
    
    return vbox(menu_elems) | border | flex;
}

Element GameTUI::renderGame() {
    updateData();
    
    if (!controller) {
        return text("Controller not found!") | color(Color::Red);
    }
    
    std::vector<Element> main_elems;
    
    std::vector<Element> top_elems;
    top_elems.push_back(text(" UNMATCHED ") | bold | color(Color::Red) | border);
    top_elems.push_back(separator());
    top_elems.push_back(text(" Turn: ") | bold | color(Color::Yellow));
    top_elems.push_back(text(current ? current->getName() : "?") | bold | color(Color::Green));
    main_elems.push_back(hbox(top_elems) | border | color(Color::GrayDark));
    
    std::vector<Element> middle_elems;
    
    std::vector<Element> left_elems;
    left_elems.push_back(renderBoard() | flex);
    left_elems.push_back(renderStatus() | flex_shrink);
    middle_elems.push_back(vbox(left_elems) | flex | border);
    
    std::vector<Element> right_elems;
    right_elems.push_back(renderPlayer(current, true) | flex_shrink);
    right_elems.push_back(separator());
    right_elems.push_back(renderPlayer(enemy, false) | flex_shrink);
    right_elems.push_back(separator());
    
    std::vector<Element> deck_elems;
    deck_elems.push_back(text(" Deck: ") | bold);
    deck_elems.push_back(text(std::to_string(current ? current->getDeck()->getdeckSize() : 0)) | color(Color::Green));
    deck_elems.push_back(text("  |  Discard: ") | bold);
    deck_elems.push_back(text(std::to_string(current ? current->getDeck()->getdiscardSize() : 0)) | color(Color::Yellow));
    right_elems.push_back(hbox(deck_elems) | border | flex_shrink);
    
    right_elems.push_back(separator());
    right_elems.push_back(renderHand(current) | flex);
    right_elems.push_back(separator());
    right_elems.push_back(renderActions() | flex_shrink);
    
    middle_elems.push_back(vbox(right_elems) | flex | border);
    
    main_elems.push_back(hbox(middle_elems) | flex);
    main_elems.push_back(renderLog() | flex_shrink);
    
    return vbox(main_elems) | flex;
}

Element GameTUI::renderBoard() {
    if (!controller) {
        return text("No board") | dim;
    }
    
    Bord& board = controller->getBoard();
    std::vector<Element> rows;
    
    for (int r = 0; r < 4; r++) {
        std::vector<Element> cols;
        for (int c = 0; c < 8; c++) {
            int pos = r * 8 + c;
            if (pos >= 32) {
                cols.push_back(text("    ") | border | size(WIDTH, EQUAL, 5));
                continue;
            }
            
            Character* ch = board.getCharacter(pos);
            std::string content = "    ";
            
            if (ch) {
                content = " " + getCharSymbol(ch) + "  ";
                if (ch->getowner() == 1) {
                    cols.push_back(text(content) | bold | color(Color::Red) | border | size(WIDTH, EQUAL, 5));
                } else {
                    cols.push_back(text(content) | bold | color(Color::Blue) | border | size(WIDTH, EQUAL, 5));
                }
            } else if (pos == 0 || pos == 12 || pos == 17 || pos == 23) {
                cols.push_back(text(" ## ") | color(Color::GrayDark) | border | size(WIDTH, EQUAL, 5));
            } else if (pos == 4 || pos == 15) {
                cols.push_back(text(" ~~ ") | color(Color::Magenta) | border | size(WIDTH, EQUAL, 5));
            } else {
                cols.push_back(text("    ") | border | size(WIDTH, EQUAL, 5));
            }
        }
        rows.push_back(hbox(cols));
    }
    
    std::vector<Element> board_elems;
    board_elems.push_back(text(" MAP ") | bold | center);
    board_elems.push_back(vbox(rows));
    
    return vbox(board_elems) | border | flex;
}

Element GameTUI::renderPlayer(Player* p, bool isCurrent) {
    if (!p || !p->getHero()) {
        std::vector<Element> elems;
        elems.push_back(text(isCurrent ? " YOU " : " ENEMY ") | bold | color(Color::GrayDark));
        elems.push_back(text("  HP: --/--") | dim);
        elems.push_back(text("  Range: --") | dim);
        elems.push_back(text("  Move: --") | dim);
        return vbox(elems) | border | size(HEIGHT, EQUAL, 6);
    }
    
    Character* hero = p->getHero();
    std::string name = hero->getName();
    
    std::vector<Element> elems;
    elems.push_back(text(" " + name + (isCurrent ? " [★]" : "") + " ") | bold);
    elems.push_back(text("  Health: " + std::to_string(hero->getHp()) + " / " + std::to_string(hero->getMaxhp())) | color(Color::Red));
    elems.push_back(text("  Range: " + std::to_string(hero->getAttacktype() == 0 ? 2 : 1)) | color(Color::Yellow));
    elems.push_back(text("  Move: " + std::to_string(hero->getMove())) | color(Color::Cyan));
    
    return vbox(elems) | border | size(HEIGHT, EQUAL, 6);
}

Element GameTUI::renderHand(Player* p) {
    if (!p || !p->getDeck()) {
        std::vector<Element> elems;
        elems.push_back(text(" HAND ") | bold | center);
        elems.push_back(text(" 0 cards") | dim | center);
        return vbox(elems) | border | flex;
    }
    
    Deck* deck = p->getDeck();
    const std::vector<Card>& hand = deck->gethand();
    std::vector<Element> cards;
    
    for (int i = 0; i < hand.size() && i < 7; i++) {
        bool sel = (i == card_select);
        cards.push_back(renderCard(hand[i], i, sel));
    }
    
    std::vector<Element> elems;
    elems.push_back(text(" " + p->getName() + " - HAND (" + std::to_string(hand.size()) + "/7) ") | bold | center);
    elems.push_back(hbox(cards) | center);
    
    return vbox(elems) | border | flex;
}

Element GameTUI::renderCard(const Card& card, int idx, bool sel) {
    Element e = vbox({
        text(" [" + std::to_string(idx + 1) + "] ") | dim | center,
        text(" " + card.getName() + " ") | bold | color(Color::Yellow),
        text(" " + card.getTypeString()) | dim,
        text(" Boost: " + std::to_string(card.getBoost())) | dim
    }) | border | size(WIDTH, EQUAL, 16);
    
    if (card.isAttack()) {
        e = e | color(Color::Red);
    } else if (card.isDefense()) {
        e = e | color(Color::Blue);
    } else if (card.isScheme()) {
        e = e | color(Color::Green);
    } else if (card.isVersatile()) {
        e = e | color(Color::Purple);
    }
    
    if (sel) {
        e = e | bold | border | color(Color::Green);
    }
    
    return e;
}

Element GameTUI::renderActions() {
    std::vector<std::string> actions = {
        "Attack",
        "Maneuver",
        "Scheme",
        "Discard",
        "Help"
    };
    
    std::vector<Element> elems;
    for (int i = 0; i < actions.size(); i++) {
        auto e = text(" " + actions[i] + " ");
        if (i == action_select) {
            e = e | bold | color(Color::Green) | border;
        }
        elems.push_back(e);
    }
    
    std::vector<Element> action_elems;
    action_elems.push_back(text(" GAME COMMANDS ") | bold | center);
    action_elems.push_back(vbox(elems));
    
    return vbox(action_elems) | border | flex_shrink;
}

Element GameTUI::renderStatus() {
    if (!current || !current->getDeck()) {
        return text(" Status: Ready") | dim;
    }
    
    Deck* deck = current->getDeck();
    
    std::vector<Element> elems;
    elems.push_back(text(" Deck: ") | bold);
    elems.push_back(text(std::to_string(deck->getdeckSize())) | color(Color::Green));
    elems.push_back(text("  |  Discard: ") | bold);
    elems.push_back(text(std::to_string(deck->getdiscardSize())) | color(Color::Yellow));
    elems.push_back(text("  |  ") | dim);
    elems.push_back(text(message) | color(Color::Cyan));
    
    return hbox(elems) | border | flex_shrink;
}

Element GameTUI::renderLog() {
    std::vector<Element> lines;
    
    int start = log.size() > 5 ? log.size() - 5 : 0;
    for (int i = start; i < log.size(); i++) {
        lines.push_back(text(" • " + log[i]) | dim);
    }
    
    std::vector<Element> elems;
    elems.push_back(text(" ACTION LOG ") | bold | color(Color::Yellow) | center);
    elems.push_back(vbox(lines));
    
    return vbox(elems) | border | flex_shrink;
}

Element GameTUI::renderCombat() {
    if (!current) {
        return text("No player data") | dim;
    }
    
    std::vector<Element> elems;
    elems.push_back(text(" COMBAT ") | bold | color(Color::Red) | center);
    elems.push_back(text(""));
    elems.push_back(text(" Choose your attack card:") | bold | color(Color::Yellow));
    elems.push_back(renderHand(current) | flex);
    elems.push_back(text(""));
    elems.push_back(text(" Select a card with ↑/↓") | dim | center);
    elems.push_back(text(" Press ENTER to confirm, ESC to cancel") | dim | center);
    
    return vbox(elems) | border | flex;
}

Element GameTUI::renderHelp() {
    std::vector<Element> elems;
    elems.push_back(text(" HELP ") | bold | color(Color::Blue) | center);
    elems.push_back(text(""));
    elems.push_back(text(" CONTROLS:") | bold | color(Color::Yellow));
    elems.push_back(text("  ↑/↓ : Navigate menus") | dim);
    elems.push_back(text("  ENTER : Select option") | dim);
    elems.push_back(text("  ESC : Go back/Cancel") | dim);
    elems.push_back(text("  Q : Quit game") | dim);
    elems.push_back(text(""));
    elems.push_back(text(" ACTIONS:") | bold | color(Color::Yellow));
    elems.push_back(text("  Attack : Fight your opponent") | dim);
    elems.push_back(text("  Maneuver : Move & draw a card") | dim);
    elems.push_back(text("  Scheme : Play special ability") | dim);
    elems.push_back(text("  Discard : Remove a card from hand") | dim);
    elems.push_back(text(""));
    elems.push_back(text(" Press ESC to return") | dim | center);
    
    return vbox(elems) | border | flex;
}

Element GameTUI::renderGameOver() {
    bool d_won = controller ? controller->get_DraculaWon() : false;
    bool s_won = controller ? controller->get_SherlockWon() : false;
    
    std::string winner = "?";
    
    if (d_won) { 
        winner = "Dracula"; 
    }
    else if (s_won) { 
        winner = "Sherlock"; 
    }
    
    std::vector<Element> elems;
    elems.push_back(text("╔════════════════════════════════╗") | bold | color(Color::Red) | center);
    elems.push_back(text("║         GAME OVER!           ║") | bold | color(Color::Red) | center);
    elems.push_back(text("╚════════════════════════════════╝") | bold | color(Color::Red) | center);
    elems.push_back(text(""));
    elems.push_back(text(" 🏆 " + winner + " WINS! 🏆 ") | bold | center);
    elems.push_back(text(""));
    elems.push_back(text(" Press ENTER to continue") | dim | center);
    
    return vbox(elems) | border | flex;
}

std::string GameTUI::getCharSymbol(Character* c) {
    if (!c) return " ";
    std::string name = c->getName();
    if (name == "Dracula") return "D";
    if (name == "Sherlock" || name == "sherlock") return "S";
    if (name.find("Sister") != std::string::npos) return "Si";
    if (name == "Dr_watson") return "W";
    return "?";
}

void GameTUI::updateData() {
    if (controller) {
        current = controller->getCurrentPlayer();
        enemy = controller->getEnemyPlayer();
    }
}

void GameTUI::refresh() {
    screen.PostEvent(Event::Custom);
}

void GameTUI::setState(State s) {
    state = s;
    refresh();
}

void GameTUI::showMessage(const std::string& msg) {
    message = msg;
    refresh();
}

void GameTUI::addLog(const std::string& msg) {
    log.push_back(msg);
    if (log.size() > 20) {
        log.erase(log.begin());
    }
    refresh();
}