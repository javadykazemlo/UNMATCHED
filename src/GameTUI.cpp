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
    
    log.push_back("🎮 Welcome to Unmatched!");
    log.push_back("⚔️ Dracula vs Sherlock Holmes");
    log.push_back("📖 Select your hero and begin the battle!");
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
        
        screen.Loop([&] { return screen_element; });
    }
}

Element GameTUI::renderMenu() {
    std::vector<std::string> items = {"🎮 Play", "📖 Help", "🚪 Exit"};
    std::vector<Element> elems;
    
    for (int i = 0; i < items.size(); i++) {
        auto e = text(items[i]);
        if (i == menu_select) {
            e = e | bold | color(Color::Green) | border(rounded);
        }
        elems.push_back(e | size(WIDTH, EQUAL, 30));
    }
    
    return vbox({
        text("╔═══════════════════════════════════════════╗") | bold | color(Color::Red),
        text("║        UNMATCHED: THE GAME              ║") | bold | color(Color::Red),
        text("║    Dracula vs Sherlock Holmes           ║") | color(Color::Yellow),
        text("╚═══════════════════════════════════════════╝") | bold | color(Color::Red),
        text(""),
        vbox(elems) | center,
        text(""),
        text(" ↑/↓: Navigate  •  ENTER: Select  •  Q: Quit") | dim | center
    }) | border | flex;
}

Element GameTUI::renderGame() {
    updateData();
    
    if (!controller) {
        return text("❌ Controller not found!") | color(Color::Red);
    }
    
    return vbox({
        hbox({
            text(" UNMATCHED ") | bold | color(Color::Red) | border,
            separator(),
            text(" Turn: ") | bold | color(Color::Yellow),
            text(current ? current->getName() : "?") | bold | color(Color::Green)
        }) | border | color(Color::GrayDark),
        
        hbox({
            vbox({
                renderBoard() | flex,
                renderStatus() | flex_shrink
            }) | flex | border,
            
            vbox({
                renderPlayer(current, true) | flex_shrink,
                separator(),
                renderPlayer(enemy, false) | flex_shrink,
                separator(),
                hbox({
                    text(" Deck: ") | bold,
                    text(std::to_string(current ? current->getDeck()->getdeckSize() : 0)) | color(Color::Green),
                    text("  |  Discard: ") | bold,
                    text(std::to_string(current ? current->getDeck()->getdiscardSize() : 0)) | color(Color::Yellow)
                }) | border | flex_shrink,
                separator(),
                renderHand(current) | flex,
                separator(),
                renderActions() | flex_shrink
            }) | flex | border
        }) | flex,
        
        renderLog() | flex_shrink
    }) | flex;
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
                Color color = (ch->getowner() == 1) ? Color::Red : Color::Blue;
                cols.push_back(text(content) | bold | color(color) | border | size(WIDTH, EQUAL, 5));
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
    
    return vbox({
        text(" MAP ") | bold | center,
        vbox(rows)
    }) | border | flex;
}

Element GameTUI::renderPlayer(Player* p, bool isCurrent) {
    if (!p || !p->getHero()) {
        return vbox({
            text(isCurrent ? " YOU " : " ENEMY ") | bold | color(Color::GrayDark),
            text("  HP: --/--") | dim,
            text("  Range: --") | dim,
            text("  Move: --") | dim
        }) | border | size(HEIGHT, EQUAL, 6);
    }
    
    Character* hero = p->getHero();
    std::string name = hero->getName();
    Color color = (hero->getowner() == 1) ? Color::Red : Color::Blue;
    
    return vbox({
        text(" " + name + (isCurrent ? " [★]" : "") + " ") | bold | color(color),
        text("  Health: " + std::to_string(hero->getHp()) + " / " + std::to_string(hero->getMaxhp())) | color(Color::Red),
        text("  Range: " + std::to_string(hero->getAttacktype() == 0 ? 2 : 1)) | color(Color::Yellow),
        text("  Move: " + std::to_string(hero->getMove())) | color(Color::Cyan)
    }) | border | size(HEIGHT, EQUAL, 6);
}

Element GameTUI::renderHand(Player* p) {
    if (!p || !p->getDeck()) {
        return vbox({
            text(" HAND ") | bold | center,
            text(" 0 cards") | dim | center
        }) | border | flex;
    }
    
    Deck* deck = p->getDeck();
    const std::vector<Card>& hand = deck->gethand();
    std::vector<Element> cards;
    
    for (int i = 0; i < hand.size() && i < 7; i++) {
        bool sel = (i == card_select);
        cards.push_back(renderCard(hand[i], i, sel));
    }
    
    return vbox({
        text(" " + p->getName() + " - HAND (" + std::to_string(hand.size()) + "/7) ") | bold | center,
        hbox(cards) | center
    }) | border | flex;
}

Element GameTUI::renderCard(const Card& card, int idx, bool sel) {
    Element e = vbox({
        text(" [" + std::to_string(idx + 1) + "] ") | dim | center,
        text(" " + card.getName() + " ") | bold | color(Color::Yellow),
        text(" " + card.getTypeString()) | dim,
        text(" Boost: " + std::to_string(card.getBoost())) | dim
    }) | border | size(WIDTH, EQUAL, 16);
    
    if (card.isAttack()) e = e | color(Color::Red);
    else if (card.isDefense()) e = e | color(Color::Blue);
    else if (card.isScheme()) e = e | color(Color::Green);
    else if (card.isVersatile()) e = e | color(Color::Purple);
    
    if (sel) e = e | bold | border(rounded) | color(Color::Green);
    
    return e;
}

Element GameTUI::renderActions() {
    std::vector<std::string> actions = {
        "⚔️ Attack",
        "🏃 Maneuver",
        "🔮 Scheme",
        "🔥 Discard",
        "❓ Help"
    };
    
    std::vector<Element> elems;
    for (int i = 0; i < actions.size(); i++) {
        auto e = text(" " + actions[i] + " ");
        if (i == action_select) {
            e = e | bold | color(Color::Green) | border(rounded);
        }
        elems.push_back(e);
    }
    
    return vbox({
        text(" GAME COMMANDS ") | bold | center,
        vbox(elems)
    }) | border | flex_shrink;
}

Element GameTUI::renderStatus() {
    if (!current || !current->getDeck()) {
        return text(" Status: Ready") | dim;
    }
    
    Deck* deck = current->getDeck();
    return hbox({
        text(" 📊 Deck: ") | bold,
        text(std::to_string(deck->getdeckSize())) | color(Color::Green),
        text("  |  Discard: ") | bold,
        text(std::to_string(deck->getdiscardSize())) | color(Color::Yellow),
        text("  |  ") | dim,
        text(message) | color(Color::Cyan)
    }) | border | flex_shrink;
}

Element GameTUI::renderLog() {
    std::vector<Element> lines;
    
    int start = log.size() > 5 ? log.size() - 5 : 0;
    for (int i = start; i < log.size(); i++) {
        lines.push_back(text(" • " + log[i]) | dim);
    }
    
    return vbox({
        text(" 📜 ACTION LOG ") | bold | color(Color::Yellow) | center,
        vbox(lines)
    }) | border | flex_shrink;
}

Element GameTUI::renderCombat() {
    if (!current) {
        return text("No player data") | dim;
    }
    
    return vbox({
        text(" ⚔️ COMBAT ⚔️ ") | bold | color(Color::Red) | center,
        text(""),
        text(" Choose your attack card:") | bold | color(Color::Yellow),
        renderHand(current) | flex,
        text(""),
        text(" 🎯 Select a card with ↑/↓") | dim | center,
        text(" Press ENTER to confirm, ESC to cancel") | dim | center
    }) | border | flex;
}

Element GameTUI::renderHelp() {
    return vbox({
        text(" 📖 HELP ") | bold | color(Color::Blue) | center,
        text(""),
        text(" 🎮 CONTROLS:") | bold | color(Color::Yellow),
        text("  ↑/↓ : Navigate menus") | dim,
        text("  ENTER : Select option") | dim,
        text("  ESC : Go back/Cancel") | dim,
        text("  Q : Quit game") | dim,
        text(""),
        text(" ⚔️ ACTIONS:") | bold | color(Color::Yellow),
        text("  Attack : Fight your opponent") | dim,
        text("  Maneuver : Move & draw a card") | dim,
        text("  Scheme : Play special ability") | dim,
        text("  Discard : Remove a card from hand") | dim,
        text(""),
        text(" 🎴 CARD TYPES:") | bold | color(Color::Yellow),
        text("  🔴 Attack : Deal damage") | color(Color::Red),
        text("  🔵 Defense : Block attacks") | color(Color::Blue),
        text("  🟢 Scheme : Special effects") | color(Color::Green),
        text("  🟣 Versatile : Attack or Defense") | color(Color::Purple),
        text(""),
        text(" Press ESC to return") | dim | center
    }) | border | flex;
}

Element GameTUI::renderGameOver() {
    bool d_won = controller ? controller->get_DraculaWon() : false;
    bool s_won = controller ? controller->get_SherlockWon() : false;
    
    std::string winner = "?";
    Color color = Color::Gray;
    
    if (d_won) { winner = "Dracula"; color = Color::Red; }
    else if (s_won) { winner = "Sherlock"; color = Color::Blue; }
    
    return vbox({
        text("╔═══════════════════════════════════════════╗") | bold | color(Color::Red) | center,
        text("║              GAME OVER!                 ║") | bold | color(Color::Red) | center,
        text("╚═══════════════════════════════════════════╝") | bold | color(Color::Red) | center,
        text(""),
        text(" 🏆 " + winner + " WINS! 🏆 ") | bold | color(color) | center,
        text(""),
        text(" Press ENTER to continue") | dim | center
    }) | border | flex;
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