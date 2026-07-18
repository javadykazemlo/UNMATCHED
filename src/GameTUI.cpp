#include "GameTUI.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <thread>
#include <chrono>
#include <cmath>

using namespace ftxui;

// ============================================
// سازنده
// ============================================
GameTUI::GameTUI(Controller* ctrl) 
    : controller(ctrl),
      screen(ScreenInteractive::Fullscreen()),
      state(MENU),
      running(true),
      menu_select(0),
      action_select(0),
      card_select(-1),
      combat_card_select(-1),
      move_target_select(-1),
      current(nullptr),
      enemy(nullptr),
      input_mode(false) {
    
    log.push_back("⚔️ Welcome to Unmatched!");
    log.push_back("🧛 Dracula vs Sherlock Holmes");
    log.push_back("📖 Select your hero and begin the battle!");
    updateData();
}

// ============================================
// حلقه اصلی با Event Handling کامل
// ============================================
void GameTUI::run() {
    auto component = Renderer([&] {
        Element screen_element;
        
        switch (state) {
            case MENU: screen_element = renderMenu(); break;
            case PLAYING: screen_element = renderGame(); break;
            case COMBAT: screen_element = renderCombat(); break;
            case CARD_SELECT: screen_element = renderCardSelect(); break;
            case HELP: screen_element = renderHelp(); break;
            case GAMEOVER: screen_element = renderGameOver(); break;
        }
        
        return screen_element | border | flex;
    });
    
    component |= CatchEvent([&](Event event) {
        return handleKeyPress(event);
    });
    
    screen.Loop(component);
}

// ============================================
// پردازش کلیدها - اصلاح‌شده برای اجرای عملیات
// ============================================
bool GameTUI::handleKeyPress(Event event) {
    // ============================
    // خروج
    // ============================
    if (event == Event::Character('q') || event == Event::Character('Q')) {
        running = false;
        screen.ExitLoopClosure()();
        return true;
    }
    
    // ============================
    // ESC
    // ============================
    if (event == Event::Escape) {
        if (state == HELP) { state = MENU; return true; }
        if (state == CARD_SELECT) { state = PLAYING; card_select = -1; return true; }
        if (state == COMBAT) { state = PLAYING; combat_card_select = -1; return true; }
        if (state == GAMEOVER) { state = MENU; return true; }
    }
    
    // ============================
    // منوی اصلی
    // ============================
    if (state == MENU) {
        if (event == Event::ArrowUp || event == Event::Character('k')) {
            menu_select = (menu_select - 1 + menu_items.size()) % menu_items.size();
            return true;
        }
        if (event == Event::ArrowDown || event == Event::Character('j')) {
            menu_select = (menu_select + 1) % menu_items.size();
            return true;
        }
        if (event == Event::Return) {
            switch (menu_select) {
                case 0: state = PLAYING; addLog("🎮 Game started!"); break;
                case 1: state = HELP; break;
                case 2: running = false; screen.ExitLoopClosure()(); break;
            }
            return true;
        }
    }
    
    // ============================
    // صفحه بازی - کلیدهای میانبر
    // ============================
    if (state == PLAYING) {
        // حرکت در منو
        if (event == Event::ArrowUp || event == Event::Character('k')) {
            action_select = (action_select - 1 + action_items.size()) % action_items.size();
            return true;
        }
        if (event == Event::ArrowDown || event == Event::Character('j')) {
            action_select = (action_select + 1) % action_items.size();
            return true;
        }
        
        // ========================================
        // کلید A - Attack (حمله)
        // ========================================
        if (event == Event::Character('a') || event == Event::Character('A')) {
            action_select = 0;
            if (controller) {
                addLog("⚔️ Starting attack...");
                controller->startCombat();
                // بعد از اتمام حمله، دوباره به حالت بازی برمی‌گردیم
                state = PLAYING;
                refresh();
            }
            return true;
        }
        
        // ========================================
        // کلید M - Maneuver (حرکت)
        // ========================================
        if (event == Event::Character('m') || event == Event::Character('M')) {
            action_select = 1;
            if (current && current->getHero()) {
                Character* hero = current->getHero();
                addLog("🏃 " + current->getName() + " is moving...");
                int boost = controller->boost();
                controller->move(hero->getMove() + boost, hero);
                addLog("✅ " + current->getName() + " moved!");
                message = "Moved successfully";
                refresh();
            }
            return true;
        }
        
        // ========================================
        // کلید S - Scheme (طرح)
        // ========================================
        if (event == Event::Character('s') || event == Event::Character('S')) {
            action_select = 2;
            if (controller) {
                addLog("🔮 Playing scheme...");
                controller->Scheme();
                addLog("✅ Scheme played!");
                refresh();
            }
            return true;
        }
        
        // ========================================
        // کلید E - End Turn (پایان نوبت)
        // ========================================
        if (event == Event::Character('e') || event == Event::Character('E')) {
            action_select = 3;
            if (controller) {
                std::swap(current, enemy);
                addLog("🔄 Turn passed to " + current->getName());
                message = current->getName() + "'s turn";
                if (controller->end_game()) {
                    state = GAMEOVER;
                }
                refresh();
            }
            return true;
        }
        
        // ========================================
        // کلید H - Help
        // ========================================
        if (event == Event::Character('h') || event == Event::Character('H')) {
            state = HELP;
            return true;
        }
        
        // ========================================
        // Space - نمایش کارت‌ها
        // ========================================
        if (event == Event::Character(' ')) {
            state = CARD_SELECT;
            card_select = 0;
            refresh();
            return true;
        }
        
        // ========================================
        // ENTER - اجرای اکشن انتخاب‌شده
        // ========================================
        if (event == Event::Return) {
            switch (action_select) {
                case 0: // Attack
                    if (controller) {
                        addLog("⚔️ Starting attack...");
                        controller->startCombat();
                        refresh();
                    }
                    break;
                case 1: // Maneuver
                    if (current && current->getHero()) {
                        Character* hero = current->getHero();
                        addLog("🏃 " + current->getName() + " is moving...");
                        int boost = controller->boost();
                        controller->move(hero->getMove() + boost, hero);
                        addLog("✅ " + current->getName() + " moved!");
                        message = "Moved successfully";
                        refresh();
                    }
                    break;
                case 2: // Scheme
                    if (controller) {
                        addLog("🔮 Playing scheme...");
                        controller->Scheme();
                        addLog("✅ Scheme played!");
                        refresh();
                    }
                    break;
                case 3: // End Turn
                    if (controller) {
                        std::swap(current, enemy);
                        addLog("🔄 Turn passed to " + current->getName());
                        message = current->getName() + "'s turn";
                        if (controller->end_game()) {
                            state = GAMEOVER;
                        }
                        refresh();
                    }
                    break;
                case 4: // Help
                    state = HELP;
                    break;
            }
            return true;
        }
    }
    
    // ============================
    // انتخاب کارت
    // ============================
    if (state == CARD_SELECT) {
        if (event == Event::ArrowUp || event == Event::Character('k')) {
            if (card_select > 0) card_select--;
            return true;
        }
        if (event == Event::ArrowDown || event == Event::Character('j')) {
            if (current && current->getDeck()) {
                int handSize = current->getDeck()->gethandSize();
                if (card_select < handSize - 1) card_select++;
            }
            return true;
        }
        if (event == Event::Return) {
            if (card_select >= 0 && current && current->getDeck()) {
                const auto& hand = current->getDeck()->gethand();
                if (card_select < (int)hand.size()) {
                    Card selected;
                    current->getDeck()->playCard(card_select, selected);
                    addLog("🃏 Played: " + selected.getName());
                    message = "Played " + selected.getName();
                    refresh();
                }
            }
            state = PLAYING;
            card_select = -1;
            refresh();
            return true;
        }
        if (event == Event::Escape) {
            state = PLAYING;
            card_select = -1;
            refresh();
            return true;
        }
    }
    
    // ============================
    // Combat
    // ============================
    if (state == COMBAT) {
        if (event == Event::ArrowUp || event == Event::Character('k')) {
            if (combat_card_select > 0) combat_card_select--;
            return true;
        }
        if (event == Event::ArrowDown || event == Event::Character('j')) {
            if (current && current->getDeck()) {
                int handSize = current->getDeck()->gethandSize();
                if (combat_card_select < handSize - 1) combat_card_select++;
            }
            return true;
        }
        if (event == Event::Return) {
            if (combat_card_select >= 0 && current && current->getDeck()) {
                const auto& hand = current->getDeck()->gethand();
                if (combat_card_select < (int)hand.size()) {
                    Card selected;
                    current->getDeck()->playCard(combat_card_select, selected);
                    addLog("⚔️ Combat card played: " + selected.getName());
                    message = "Combat: " + selected.getName();
                    refresh();
                }
            }
            state = PLAYING;
            combat_card_select = -1;
            refresh();
            return true;
        }
        if (event == Event::Escape) {
            state = PLAYING;
            combat_card_select = -1;
            refresh();
            return true;
        }
    }
    
    // ============================
    // Command Mode
    // ============================
    if (event == Event::Character(':')) {
        input_mode = true;
        input_buffer = ":";
        return true;
    }
    
    if (input_mode) {
        if (event == Event::Return) {
            input_mode = false;
            if (input_buffer.size() > 1) {
                std::string cmd = input_buffer.substr(1);
                processCommand(cmd);
            }
            input_buffer.clear();
            return true;
        }
        if (event == Event::Escape) {
            input_mode = false;
            input_buffer.clear();
            return true;
        }
        if (event.is_character()) {
            input_buffer += event.character();
            return true;
        }
        if (event == Event::Backspace && !input_buffer.empty()) {
            input_buffer.pop_back();
            return true;
        }
    }
    
    return false;
}

// ============================================
// رندر منوی اصلی
// ============================================
Element GameTUI::renderMenu() {
    std::vector<Element> elems;
    
    elems.push_back(text("╔═══════════════════════════════════════════╗") | bold | color(Color::Red));
    elems.push_back(text("║        🦇 UNMATCHED: THE GAME 🕵️       ║") | bold | color(Color::Red));
    elems.push_back(text("║    Dracula vs Sherlock Holmes           ║") | color(Color::Yellow));
    elems.push_back(text("╚═══════════════════════════════════════════╝") | bold | color(Color::Red));
    elems.push_back(text("") | size(WIDTH, EQUAL, 1));
    
    for (int i = 0; i < (int)menu_items.size(); i++) {
        auto e = text("  " + menu_items[i] + "  ");
        if (i == menu_select) {
            e = e | bold | color(Color::Green) | border | size(WIDTH, EQUAL, 15);
        } else {
            e = e | size(WIDTH, EQUAL, 15);
        }
        elems.push_back(e | center);
    }
    
    elems.push_back(text(""));
    elems.push_back(text(" ↑/↓: Navigate  •  ENTER: Select  •  Q: Quit") | dim | center);
    
    return vbox(elems) | border | flex | center;
}

// ============================================
// رندر صفحه بازی - نقشه بزرگ‌تر، کارت‌ها راست‌تر
// ============================================
Element GameTUI::renderGame() {
    updateData();
    
    if (!controller) {
        return text("❌ Controller not found!") | color(Color::Red);
    }
    
    std::vector<Element> main_elems;
    
    // ========================================
    // نوار بالایی
    // ========================================
    std::vector<Element> top_elems;
    top_elems.push_back(text(" UNMATCHED ") | bold | color(Color::Red));
    top_elems.push_back(separator());
    top_elems.push_back(text(" Turn: ") | bold | color(Color::Yellow));
    top_elems.push_back(text(current ? current->getName() : "?") | bold | color(Color::Green));
    top_elems.push_back(separator());
    top_elems.push_back(text("[A]ttack [M]ove [S]cheme [E]nd [H]elp [Space]Hand") | dim);
    
    if (input_mode) {
        top_elems.push_back(text(" CMD: ") | color(Color::Cyan));
        top_elems.push_back(text(input_buffer) | color(Color::White) | bold);
    }
    
    main_elems.push_back(hbox(top_elems) | border | color(Color::GrayDark));
    
    // ========================================
    // بخش میانی - نقشه (65%) و کارت‌ها (35%)
    // ========================================
    std::vector<Element> middle_elems;
    
    // نقشه - بزرگ‌تر (65%)
    middle_elems.push_back(renderBoard() | flex | size(WIDTH, EQUAL, 65) | border);
    
    // سمت راست - کارت‌ها و اطلاعات (35%)
    std::vector<Element> right_elems;
    right_elems.push_back(renderPlayer(current, true) | flex_shrink);
    right_elems.push_back(separator());
    right_elems.push_back(renderPlayer(enemy, false) | flex_shrink);
    right_elems.push_back(separator());
    right_elems.push_back(renderStatus() | flex_shrink);
    right_elems.push_back(separator());
    right_elems.push_back(renderHand(current) | flex);
    middle_elems.push_back(vbox(right_elems) | flex | size(WIDTH, EQUAL, 35) | border);
    
    main_elems.push_back(hbox(middle_elems) | flex);
    
    // ========================================
    // بخش پایین - اکشن‌ها و لاگ
    // ========================================
    std::vector<Element> bottom_elems;
    bottom_elems.push_back(renderActions() | flex_shrink);
    bottom_elems.push_back(separator());
    bottom_elems.push_back(renderLog() | flex_shrink);
    main_elems.push_back(vbox(bottom_elems) | border);
    
    return vbox(main_elems) | flex;
}

// ============================================
// رندر نقشه - خانه‌های کوچک با خط اتصال
// ============================================
Element GameTUI::renderBoard() {
    if (!controller) {
        return text("No board") | dim;
    }
    
    Bord& board = controller->getBoard();
    
    // چیدمان 4×8 با خانه‌های کوچک
    std::vector<std::vector<int>> grid = {
        {0, 1, 2, 3, 4, 5, 6, 7},
        {8, 9, 10, 11, 12, 13, 14, 15},
        {16, 17, 18, 19, 20, 21, 22, 23},
        {24, 25, 26, 27, 28, 29, 30, 31}
    };
    
    std::vector<Element> allRows;
    const int CELL_WIDTH = 3; // خانه کوچک
    
    for (int r = 0; r < 4; r++) {
        std::vector<Element> rowCells;
        
        for (int c = 0; c < 8; c++) {
            int pos = grid[r][c];
            if (pos >= 32) {
                rowCells.push_back(text("   ") | size(WIDTH, EQUAL, CELL_WIDTH));
                continue;
            }
            
            Character* ch = board.getCharacter(pos);
            Element cell;
            std::string display;
            
            if (ch) {
                std::string name = ch->getName();
                
                if (name == "Dracula") {
                    display = "D";
                } else if (name == "sherlock" || name == "Sherlock") {
                    display = "S";
                } else if (name == "Dr_watson") {
                    display = "w";
                } else if (name.find("Sister") != std::string::npos) {
                    display = "s";
                } else {
                    display = "?";
                }
                
                if (ch->getowner() == 1) {
                    cell = text(display) | bold | color(Color::Red) | 
                           border | size(WIDTH, EQUAL, CELL_WIDTH);
                } else {
                    cell = text(display) | bold | color(Color::Blue) | 
                           border | size(WIDTH, EQUAL, CELL_WIDTH);
                }
            } else {
                // خانه خالی با خط زیرین _
                if (pos == 0 || pos == 12 || pos == 17 || pos == 23) {
                    cell = text(" # ") | color(Color::Magenta) | 
                           border | size(WIDTH, EQUAL, CELL_WIDTH);
                } else if (pos == 4 || pos == 15) {
                    cell = text(" ~ ") | color(Color::Yellow) | 
                           border | size(WIDTH, EQUAL, CELL_WIDTH);
                } else {
                    // خانه عادی با خط زیرین
                    cell = text(" _ ") | color(Color::GrayDark) | 
                           border | size(WIDTH, EQUAL, CELL_WIDTH);
                }
            }
            rowCells.push_back(cell);
        }
        allRows.push_back(hbox(rowCells));
        
        // ========================================
        // خطوط اتصال بین خانه‌ها با -
        // ========================================
        if (r < 3) {
            std::vector<Element> connectors;
            for (int c = 0; c < 8; c++) {
                int pos = grid[r][c];
                int belowPos = grid[r + 1][c];
                
                auto adj = board.getposAdjacent(pos);
                bool connected = std::find(adj.begin(), adj.end(), belowPos) != adj.end();
                
                connectors.push_back(
                    text(connected ? " | " : "   ") | 
                    color(connected ? Color::GrayLight : Color::GrayDark) | 
                    size(WIDTH, EQUAL, CELL_WIDTH)
                );
            }
            allRows.push_back(hbox(connectors) | size(HEIGHT, EQUAL, 1));
        }
    }
    
    std::vector<Element> board_elems;
    board_elems.push_back(text(" 🗺️ MAP ") | bold | color(Color::Cyan) | center);
    board_elems.push_back(vbox(allRows) | center);
    
    // راهنما
    std::vector<Element> legend;
    legend.push_back(text("D=Dracula") | color(Color::Red) | size(WIDTH, EQUAL, 9));
    legend.push_back(text("S=Sherlock") | color(Color::Blue) | size(WIDTH, EQUAL, 9));
    legend.push_back(text("w=Watson") | color(Color::Cyan) | size(WIDTH, EQUAL, 8));
    legend.push_back(text("s=Sister") | color(Color::Magenta) | size(WIDTH, EQUAL, 8));
    legend.push_back(text("#=Secret") | color(Color::Yellow) | size(WIDTH, EQUAL, 8));
    legend.push_back(text("~=Start") | color(Color::Green) | size(WIDTH, EQUAL, 8));
    board_elems.push_back(hbox(legend) | dim | center);
    
    return vbox(board_elems) | border | flex;
}

// ============================================
// رندر اطلاعات بازیکن
// ============================================
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
    
    std::string header = (isCurrent ? " " : " ") + name;
    if (isCurrent) header += " [★]";
    elems.push_back(text(" " + header + " ") | bold | color(isCurrent ? Color::Green : Color::Red));
    
    std::string hpBar = getHPBar(hero->getHp(), hero->getMaxhp());
    elems.push_back(text("  HP: " + std::to_string(hero->getHp()) + "/" + std::to_string(hero->getMaxhp()) + " " + hpBar) | color(Color::Red));
    
    elems.push_back(text("  Range: " + std::to_string(hero->getAttacktype() == 0 ? 2 : 1) + "  Move: " + std::to_string(hero->getMove())) | color(Color::Yellow));
    
    if (p->getDeck()) {
        elems.push_back(text("  Hand: " + std::to_string(p->getDeck()->gethandSize()) + "/5  Deck: " + std::to_string(p->getDeck()->getdeckSize())) | dim);
    }
    
    elems.push_back(text("  Loc: " + std::to_string(hero->getSpace())) | color(Color::Cyan));
    
    return vbox(elems) | border | size(HEIGHT, EQUAL, 7);
}

// ============================================
// رندر دست کارت‌ها
// ============================================
Element GameTUI::renderHand(Player* p, int selected) {
    if (!p || !p->getDeck()) {
        std::vector<Element> elems;
        elems.push_back(text(" HAND ") | bold | center);
        elems.push_back(text(" 0 cards") | dim | center);
        return vbox(elems) | border | flex;
    }
    
    Deck* deck = p->getDeck();
    const std::vector<Card>& hand = deck->gethand();
    std::vector<Element> cards;
    
    if (hand.empty()) {
        cards.push_back(text(" No cards") | dim | center);
    } else {
        int displayCount = std::min(4, (int)hand.size());
        for (int i = 0; i < displayCount; i++) {
            bool sel = (i == selected);
            cards.push_back(renderCard(hand[i], i, sel));
        }
    }
    
    std::vector<Element> elems;
    elems.push_back(text(" " + p->getName() + " - HAND (" + std::to_string(hand.size()) + "/5) ") | 
                    bold | color(Color::Yellow) | center);
    elems.push_back(hbox(cards) | center);
    
    return vbox(elems) | border | flex;
}

// ============================================
// رندر یک کارت
// ============================================
Element GameTUI::renderCard(const Card& card, int idx, bool sel) {
    std::vector<Element> elems;
    
    elems.push_back(text(" [" + std::to_string(idx + 1) + "] ") | dim);
    
    std::string typeLabel;
    Color typeColor;
    if (card.isAttack()) {
        typeLabel = "ATTACK";
        typeColor = Color::Red;
    } else if (card.isDefense()) {
        typeLabel = "DEFENSE";
        typeColor = Color::Blue;
    } else if (card.isScheme()) {
        typeLabel = "SCHEME";
        typeColor = Color::Green;
    } else {
        typeLabel = "VERSATILE";
        typeColor = Color::Magenta;
    }
    elems.push_back(text(" " + typeLabel + " ") | bold | color(typeColor));
    
    elems.push_back(text(" " + card.getName() + " ") | bold | color(Color::White));
    
    if (card.getAttack() > 0) {
        elems.push_back(text(" Atk:" + std::to_string(card.getAttack())) | color(Color::Red));
    }
    elems.push_back(text(" Boost:" + std::to_string(card.getBoost())) | color(Color::Yellow));
    elems.push_back(text(" " + card.getTimingString()) | dim);
    
    std::string effect = card.geteffect();
    if (effect.length() > 15) {
        effect = effect.substr(0, 15) + "...";
    }
    if (!effect.empty()) {
        elems.push_back(text(" " + effect) | color(Color::Cyan) | italic);
    }
    
    Element card_elem = vbox(elems) | border | size(WIDTH, EQUAL, 18);
    
    if (sel) {
        card_elem = card_elem | bold | border | color(Color::Green);
    }
    
    return card_elem;
}

// ============================================
// رندر منوی اکشن‌ها
// ============================================
Element GameTUI::renderActions() {
    std::vector<Element> elems;
    
    for (int i = 0; i < (int)action_items.size(); i++) {
        auto e = text(" " + action_items[i] + " ");
        if (i == action_select) {
            e = e | bold | color(Color::Green) | border;
        }
        elems.push_back(e);
    }
    
    std::vector<Element> action_elems;
    action_elems.push_back(text(" ACTIONS ") | bold | color(Color::Yellow) | center);
    action_elems.push_back(hbox(elems) | center);
    action_elems.push_back(text(" ↑/↓: Navigate  ENTER: Execute  :: Command") | dim | center);
    
    return vbox(action_elems) | border | flex_shrink;
}

// ============================================
// رندر وضعیت
// ============================================
Element GameTUI::renderStatus() {
    if (!current || !current->getDeck()) {
        return text(" Status: Ready") | dim;
    }
    
    Deck* deck = current->getDeck();
    
    std::vector<Element> elems;
    elems.push_back(text(" STATUS ") | bold | color(Color::Cyan));
    
    std::vector<Element> info;
    info.push_back(text("Deck: ") | bold);
    info.push_back(text(std::to_string(deck->getdeckSize())) | color(Color::Green));
    info.push_back(text("  Discard: ") | bold);
    info.push_back(text(std::to_string(deck->getdiscardSize())) | color(Color::Yellow));
    info.push_back(text("  ") | dim);
    info.push_back(text(message) | color(Color::Cyan));
    
    elems.push_back(hbox(info));
    
    return vbox(elems) | border | flex_shrink;
}

// ============================================
// رندر لاگ
// ============================================
Element GameTUI::renderLog() {
    std::vector<Element> lines;
    
    int start = log.size() > 4 ? log.size() - 4 : 0;
    for (int i = start; i < (int)log.size(); i++) {
        lines.push_back(text(" " + log[i]) | dim);
    }
    
    std::vector<Element> elems;
    elems.push_back(text(" ACTION LOG ") | bold | color(Color::Yellow));
    elems.push_back(vbox(lines));
    
    return vbox(elems) | border | flex_shrink | size(HEIGHT, EQUAL, 7);
}

// ============================================
// رندر صفحه راهنما
// ============================================
Element GameTUI::renderHelp() {
    std::vector<Element> elems;
    elems.push_back(text(" HELP ") | bold | color(Color::Blue) | center);
    elems.push_back(text(""));
    elems.push_back(text(" CONTROLS:") | bold | color(Color::Yellow));
    elems.push_back(text("  ↑/↓ or j/k : Navigate") | dim);
    elems.push_back(text("  ENTER : Select") | dim);
    elems.push_back(text("  ESC : Back/Cancel") | dim);
    elems.push_back(text("  Q : Quit") | dim);
    elems.push_back(text("  : : Command mode") | dim);
    elems.push_back(text(""));
    elems.push_back(text(" ACTIONS:") | bold | color(Color::Yellow));
    elems.push_back(text("  A - Attack") | dim);
    elems.push_back(text("  M - Maneuver") | dim);
    elems.push_back(text("  S - Scheme") | dim);
    elems.push_back(text("  E - End Turn") | dim);
    elems.push_back(text(""));
    elems.push_back(text(" COMMANDS:") | bold | color(Color::Yellow));
    elems.push_back(text("  :move <pos>") | dim);
    elems.push_back(text("  :play <idx>") | dim);
    elems.push_back(text("  :hand") | dim);
    elems.push_back(text("  :deck") | dim);
    elems.push_back(text(""));
    elems.push_back(text(" Press ESC to return") | dim | center);
    
    return vbox(elems) | border | flex;
}

// ============================================
// رندر صفحه پایان بازی
// ============================================
Element GameTUI::renderGameOver() {
    bool d_won = controller ? controller->get_DraculaWon() : false;
    bool s_won = controller ? controller->get_SherlockWon() : false;
    
    std::string winner = "?";
    std::string emoji = "❓";
    
    if (d_won) { 
        winner = "Dracula";
        emoji = "🧛";
    } else if (s_won) { 
        winner = "Sherlock";
        emoji = "🕵️";
    }
    
    std::vector<Element> elems;
    elems.push_back(text("╔════════════════════════════════╗") | bold | color(Color::Red));
    elems.push_back(text("║         GAME OVER!           ║") | bold | color(Color::Red));
    elems.push_back(text("╚════════════════════════════════╝") | bold | color(Color::Red));
    elems.push_back(text(""));
    elems.push_back(text(" " + emoji + " " + winner + " WINS! " + emoji + " ") | bold | color(Color::Green) | center);
    elems.push_back(text(""));
    elems.push_back(text(" Press ENTER") | dim | center);
    
    return vbox(elems) | border | flex;
}

// ============================================
// رندر انتخاب کارت (Combat)
// ============================================
Element GameTUI::renderCombat() {
    if (!current) {
        return text("No player data") | dim;
    }
    
    std::vector<Element> elems;
    elems.push_back(text(" COMBAT ") | bold | color(Color::Red) | center);
    elems.push_back(text(""));
    elems.push_back(text(" Choose attack card:") | bold | color(Color::Yellow));
    elems.push_back(renderHand(current, combat_card_select) | flex);
    elems.push_back(text(""));
    elems.push_back(text(" ↑/↓: Select  ENTER: Confirm  ESC: Cancel") | dim | center);
    
    return vbox(elems) | border | flex;
}

// ============================================
// رندر انتخاب کارت (General)
// ============================================
Element GameTUI::renderCardSelect() {
    if (!current) {
        return text("No player data") | dim;
    }
    
    std::vector<Element> elems;
    elems.push_back(text(" SELECT CARD ") | bold | color(Color::Yellow) | center);
    elems.push_back(text(""));
    elems.push_back(renderHand(current, card_select) | flex);
    elems.push_back(text(""));
    elems.push_back(text(" ↑/↓: Select  ENTER: Play  ESC: Cancel") | dim | center);
    
    return vbox(elems) | border | flex;
}

// ============================================
// پردازش دستورات Command Mode
// ============================================
void GameTUI::processCommand(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string command;
    iss >> command;
    
    if (command == "move" || command == "m") {
        int pos;
        if (iss >> pos) {
            if (current && current->getHero()) {
                controller->move(current->getHero()->getMove(), current->getHero());
                addLog("🏃 Moved to " + std::to_string(pos));
                message = "Moved to " + std::to_string(pos);
            }
        } else {
            addLog("❌ Usage: move <pos>");
        }
    } else if (command == "play" || command == "p") {
        int idx;
        if (iss >> idx) {
            if (current && current->getDeck()) {
                const auto& hand = current->getDeck()->gethand();
                if (idx > 0 && idx <= (int)hand.size()) {
                    Card selected;
                    current->getDeck()->playCard(idx - 1, selected);
                    addLog("🃏 Played: " + selected.getName());
                    message = "Played " + selected.getName();
                } else {
                    addLog("❌ Invalid index");
                }
            }
        } else {
            addLog("❌ Usage: play <idx>");
        }
    } else if (command == "hand" || command == "h") {
        state = CARD_SELECT;
        card_select = 0;
        refresh();
        addLog("📋 Showing hand...");
    } else if (command == "deck" || command == "d") {
        if (current && current->getDeck()) {
            std::string info = "Deck: " + std::to_string(current->getDeck()->getdeckSize()) + 
                               " | Hand: " + std::to_string(current->getDeck()->gethandSize()) +
                               " | Discard: " + std::to_string(current->getDeck()->getdiscardSize());
            addLog("📊 " + info);
            message = info;
        }
    } else if (command == "help") {
        state = HELP;
        refresh();
    } else if (command == "quit") {
        running = false;
        screen.ExitLoopClosure()();
    } else {
        addLog("❌ Unknown: " + command);
        addLog("   Available: move, play, hand, deck, help, quit");
    }
    refresh();
}

// ============================================
// توابع کمکی
// ============================================
std::string GameTUI::getCharSymbol(Character* c) {
    if (!c) return " ";
    std::string name = c->getName();
    if (name == "Dracula") return "D";
    if (name == "Sherlock" || name == "sherlock") return "S";
    if (name.find("Sister") != std::string::npos) return "s";
    if (name == "Dr_watson") return "w";
    return "?";
}

std::string GameTUI::getHPBar(int current, int max) {
    int barLength = 8;
    int filled = (current * barLength) / max;
    if (filled < 0) filled = 0;
    if (filled > barLength) filled = barLength;
    std::string bar = "[";
    for (int i = 0; i < barLength; i++) {
        if (i < filled) bar += "#";
        else bar += " ";
    }
    bar += "]";
    return bar;
}

std::string GameTUI::centerText(const std::string& text, int width) {
    int padding = (width - text.length()) / 2;
    if (padding < 0) padding = 0;
    return std::string(padding, ' ') + text + std::string(width - text.length() - padding, ' ');
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