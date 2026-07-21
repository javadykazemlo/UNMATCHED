#include "GameTUI.hpp"
#include "core/Player.hpp"
#include "core/Bord.hpp"
#include "entities/Character.hpp"
#include "cards/Deck.hpp"
#include "cards/Card.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <iostream>
#include <algorithm>
#include <sstream>
#include <cmath>

using namespace std;
using namespace ftxui;


void GameTUI::showWelcome()
{
    using ftxui::Element;

    auto screen = ftxui::ScreenInteractive::Fullscreen();

    Element title = ftxui::text("U N M A T C H E D");
    title = ftxui::bold(title);
    title = ftxui::color(ftxui::Color::RedLight, title);
    title = ftxui::center(title);

    Element subtitle = ftxui::text("Welcome to Unmatched. Enjoy the game!");
    subtitle = ftxui::color(ftxui::Color::White, subtitle);
    subtitle = ftxui::center(subtitle);

    Element hint = ftxui::text("Press ENTER to continue...");
    hint = ftxui::dim(hint);
    hint = ftxui::color(ftxui::Color::GrayLight, hint);
    hint = ftxui::center(hint);

    Element line = ftxui::color(ftxui::Color::Red, ftxui::separator());

    Element content = ftxui::vbox({
        ftxui::text(""),
        title,
        ftxui::text(""),
        line,
        ftxui::text(""),
        subtitle,
        ftxui::text(""),
        ftxui::text(""),
        hint,
    });
    content = ftxui::center(content);

    Element framed = ftxui::border(content);
    framed = ftxui::color(ftxui::Color::Red, framed);
    framed = ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 60)(framed);
    framed = ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 15)(framed);
    framed = ftxui::center(framed);

    auto renderer = ftxui::Renderer([&] { return framed; });

    auto withEnter = ftxui::CatchEvent(renderer, [&](ftxui::Event event) {
        if (event == ftxui::Event::Return) {
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(withEnter);
    system("clear");
}
// purely cosmetic: the engine stores some hero names in lowercase
// ("sherlock"); capitalize just for display, without touching game data.
static string displayName(const string& s)
{
    if (s.empty()) return s;
    string out = s;
    out[0] = toupper((unsigned char)out[0]);
    return out;
}

const string GameTUI::RESET   = "\033[0m";
const string GameTUI::BOLD    = "\033[1m";
const string GameTUI::DIM     = "\033[2m";
const string GameTUI::RED     = "\033[31m";
const string GameTUI::BLUE    = "\033[34m";
const string GameTUI::GREEN   = "\033[32m";
const string GameTUI::YELLOW  = "\033[33m";
const string GameTUI::CYAN    = "\033[36m";
const string GameTUI::MAGENTA = "\033[35m";
const string GameTUI::GREY    = "\033[90m";

// ─────────────────────────── small helpers ────────────────────────────

void GameTUI::clearScreen()
{
    cout << "\033[2J\033[H";
}

int GameTUI::visibleLength(const string& s)
{
    int len = 0;
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == '\033')
        {
            while (i < s.size() && s[i] != 'm') i++;
            continue;
        }
        // Rough UTF-8 aware count: only count the first byte of a
        // multi-byte sequence so box-drawing / unicode glyphs count as 1.
        unsigned char c = static_cast<unsigned char>(s[i]);
        if ((c & 0xC0) != 0x80) len++;
    }
    return len;
}

string GameTUI::padVisible(const string& s, int width)
{
    int len = visibleLength(s);
    if (len >= width) return s;
    return s + string(width - len, ' ');
}

string GameTUI::center(const string& s, int width)
{
    int len = visibleLength(s);
    if (len >= width) return s;
    int left = (width - len) / 2;
    int right = width - len - left;
    return string(left, ' ') + s + string(right, ' ');
}

vector<string> GameTUI::wrapText(const string& text, int width)
{
    vector<string> lines;
    string word, line;
    stringstream ss(text);
    while (ss >> word)
    {
        if ((int)(line.size() + word.size() + 1) > width)
        {
            lines.push_back(line);
            line = word;
        }
        else
        {
            line = line.empty() ? word : line + " " + word;
        }
    }
    if (!line.empty()) lines.push_back(line);
    return lines;
}

void GameTUI::printSideBySide(const vector<string>& left,
                               const vector<string>& mid,
                               const vector<string>& right)
{
    size_t rows = max({left.size(), mid.size(), right.size()});

    int lw = 0, mw = 0, rw = 0;
    for (auto& l : left)  lw = max(lw, visibleLength(l));
    for (auto& l : mid)   mw = max(mw, visibleLength(l));
    for (auto& l : right) rw = max(rw, visibleLength(l));

    for (size_t i = 0; i < rows; i++)
    {
        string l = i < left.size()  ? left[i]  : "";
        string m = i < mid.size()   ? mid[i]   : "";
        string r = i < right.size() ? right[i] : "";
        cout << padVisible(l, lw) << "  " << padVisible(m, mw) << "  " << padVisible(r, rw) << "\n";
    }
}

// ─────────────────────────────── map ───────────────────────────────

GameTUI::NodePos GameTUI::nodePos(int id)
{
    // col/row here are logical layout units (not a strict grid) —
    // they mirror the real board's regions: misty grounds (top-left),
    // study/library (top), sitting room + stairs hub (middle),
    // dining/hallway (right of center), outer path (bottom row).
    static const NodePos table[32] = {
        /*0*/  {1,0},  /*1*/  {1,2},  /*2*/  {4,0},  /*3*/  {5,2},
        /*4*/  {3,3},  /*5*/  {6,3},  /*6*/  {4,5},  /*7*/  {4,7},
        /*8*/  {6,6},  /*9*/  {7,5},  /*10*/ {9,4},  /*11*/ {9,6},
        /*12*/ {11,4}, /*13*/ {8,8},  /*14*/ {7,10}, /*15*/ {9,10},
        /*16*/ {10,7}, /*17*/ {12,6}, /*18*/ {13,8}, /*19*/ {14,10},
        /*20*/ {6,5},  /*21*/ {3,5},  /*22*/ {10,10},/*23*/ {9,12},
        /*24*/ {11,12},/*25*/ {13,12},/*26*/ {15,11},/*27*/ {15,13},
        /*28*/ {17,12},/*29*/ {19,12},/*30*/ {21,13},/*31*/ {20,11},
    };
    return table[id];
}

bool GameTUI::isSecretPassage(int id)
{
    return id == 0 || id == 12 || id == 17 || id == 23;
}

string GameTUI::nodeGlyph(Bord& bord, int id, Player& dracula, Player& sherlock)
{
    Character* occ = bord.getCharacter(id);

    if (occ != nullptr)
    {
        bool isDraculaSide = false;
        for (Character* c : dracula.getCharacters())
            if (c == occ) { isDraculaSide = true; break; }

        string glyph;
        string name = occ->getName();

        if (occ->isHero())
        {
            // heroes always show as D (Dracula side) or S (Sherlock side)
            glyph = isDraculaSide ? "D " : "S ";
        }
        else
        {
            // sidekick: use first letter of the last name token
            char letter = '?';
            if (!name.empty())
            {
                size_t sp = name.find_last_of(' ');
                letter = (sp == string::npos) ? name[0] : name[sp + 1];
            }
            glyph = string(1, (char)toupper((unsigned char)letter)) + " ";
        }

        string color = isDraculaSide ? RED : BLUE;
        return color + BOLD + glyph + RESET;
    }

    if (isSecretPassage(id))
        return YELLOW + "##" + RESET;

    char buf[8];
    snprintf(buf, sizeof(buf), "%2d", id);
    return GREY + string(buf) + RESET;
}

// --- internal helper: draws a clean "diagonal then straight" line between
// two canvas points, so edges never depend on nodes being grid-neighbors. ---
static void drawEdgeOnCanvas(vector<vector<string>>& canvas, int W, int H,
                              int x0, int y0, int x1, int y1)
{
    const std::string RESET = "\033[0m";
    const std::string GREY  = "\033[90m";
    int dx = x1 - x0, dy = y1 - y0;
    int stepx = (dx > 0) - (dx < 0);
    int stepy = (dy > 0) - (dy < 0);
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;
    int n = adx < ady ? adx : ady;

    string diag = (stepx == stepy) ? "\\" : "/";
    int cx = x0, cy = y0;

    for (int k = 0; k < n; k++)
    {
        cx += stepx; cy += stepy;
        if (cx >= 0 && cx < W && cy >= 0 && cy < H && canvas[cy][cx] == " ")
            canvas[cy][cx] = GREY + diag + RESET;
    }
    while (cx != x1)
    {
        cx += stepx;
        if (cx >= 0 && cx < W && cy >= 0 && cy < H && canvas[cy][cx] == " ")
            canvas[cy][cx] = GREY + string("-") + RESET;
    }
    while (cy != y1)
    {
        cy += stepy;
        if (cx >= 0 && cx < W && cy >= 0 && cy < H && canvas[cy][cx] == " ")
            canvas[cy][cx] = GREY + string("|") + RESET;
    }
}

vector<string> GameTUI::buildMapLines(Bord& bord, Player& dracula, Player& sherlock)
{
    // pixel-space scale: 3 cols per logical unit, 2 rows per logical unit
    auto px = [](int col) { return 1 + col * 3; };
    auto py = [](int row) { return 1 + row * 2; };

    int maxCol = 0, maxRow = 0;
    for (int id = 0; id < 32; id++)
    {
        NodePos p = nodePos(id);
        maxCol = max(maxCol, p.col);
        maxRow = max(maxRow, p.row);
    }

    int W = px(maxCol) + 3;
    int H = py(maxRow) + 2;

    vector<vector<string>> canvas(H, vector<string>(W, " "));

    // draw edges first so labels always render on top
    for (int id = 0; id < 32; id++)
    {
        NodePos a = nodePos(id);
        for (int nb : bord.getposAdjacent(id))
        {
            if (nb <= id) continue; // avoid drawing each edge twice
            NodePos b = nodePos(nb);
            drawEdgeOnCanvas(canvas, W, H, px(a.col), py(a.row), px(b.col), py(b.row));
        }
    }
    // stamp node glyphs on top (each glyph is 2 visible chars wide)
    for (int id = 0; id < 32; id++)
    {
        NodePos p = nodePos(id);
        int cx = px(p.col), cy = py(p.row);
        if (cy < 0 || cy >= H || cx < 0 || cx + 1 >= W) continue;
        canvas[cy][cx] = nodeGlyph(bord, id, dracula, sherlock);
        canvas[cy][cx + 1] = ""; // glyph already includes both visible chars
    }

    vector<string> raw;
    for (int r = 0; r < H; r++)
    {
        string line;
        for (int c = 0; c < W; c++)
            line += canvas[r][c];
        raw.push_back(line);
    }

    // fence-style frame around just the grid itself
    int innerWidth = 0;
    for (auto& l : raw) innerWidth = max(innerWidth, visibleLength(l));

    vector<string> lines;
    lines.push_back(BOLD + CYAN + "MAP (Graph)" + RESET);
    lines.push_back("");

    lines.push_back(GREY + "+" + string(innerWidth + 2, '=') + "+" + RESET);
    for (auto& l : raw)
        lines.push_back(GREY + "| " + RESET + padVisible(l, innerWidth) + GREY + " |" + RESET);
    lines.push_back(GREY + "+" + string(innerWidth + 2, '=') + "+" + RESET);

    lines.push_back("");
    lines.push_back(DIM + "Legend:" + RESET);
    lines.push_back(RED + BOLD + "D" + RESET + DIM + " Dracula   " + RESET +
                     BLUE + BOLD + "S" + RESET + DIM + " Sherlock" + RESET);
    lines.push_back(DIM + "Letters = sidekicks   " + RESET + YELLOW + "##" + RESET +
                     DIM + " = secret passage (0, 12, 17, 23 all connect to each other)" + RESET);
    lines.push_back(DIM + "Numbers = empty space id" + RESET);
    return lines;
}

// ────────────────────────── hero / hand panels ──────────────────────────

vector<string> GameTUI::buildHeroPanel(Player& player, bool isTurn, const string& color, int width)
{
    vector<string> lines;
    Character* hero = player.getHero();

    int inner = width - 4;
    string top = "┌" + string(width - 2, '-') + "┐";
    string bot = "└" + string(width - 2, '-') + "┘";
    string sep = "│" + string(width - 2, '-') + "│";

    lines.push_back(top);

    string title = color + BOLD + displayName(hero->getName()) + RESET;
    if (isTurn) title += "  " + YELLOW + "* TURN *" + RESET;
    lines.push_back("│ " + padVisible(title, inner) + " │");

    lines.push_back("│ " + padVisible("Health: " + to_string(hero->getHp()) + " / " + to_string(hero->getMaxhp()), inner) + " │");

    int maxBar = min(inner - 2, 20);
    int filled = hero->getMaxhp() > 0 ? (int)round((double)hero->getHp() / hero->getMaxhp() * maxBar) : 0;
    string bar = "[" + color + string(filled, '#') + RESET + string(maxBar - filled, ' ') + "]";
    lines.push_back("│ " + padVisible(bar, inner) + " │");

    lines.push_back("│ " + padVisible(string(), inner) + " │");
    string atk = hero->getAttacktype() == 0 ? "Melee" : "Ranged";
    lines.push_back("│ " + padVisible("Type: " + atk + "    Move: " + to_string(hero->getMove()), inner) + " │");

    lines.push_back(sep);
    lines.push_back("│ " + padVisible("Deck (Draw Pile): " + to_string(player.getDeck()->getdeckSize()), inner) + " │");
    lines.push_back("│ " + padVisible("Discard Pile: " + to_string(player.getDeck()->getdiscardSize()), inner) + " │");

    lines.push_back(sep);
    lines.push_back("│ " + padVisible(string(DIM + "Team:" + RESET), inner) + " │");

    for (Character* c : player.getCharacters())
    {
        if (c == hero) continue;
        string status = c->checkalive()
            ? to_string(c->getHp()) + "/" + to_string(c->getMaxhp())
            : (GREY + "K.O." + RESET);
        string entry = "  " + c->getName() + "  " + status;
        lines.push_back("│ " + padVisible(entry, inner) + " │");
    }

    lines.push_back(bot);
    return lines;
}

vector<string> GameTUI::buildHandPanel(Player& player, const string& color, int width)
{
    vector<string> lines;
    const vector<Card>& hand = player.getDeck()->gethand();

    string header = color + BOLD + player.getName() + " - HAND (" + to_string(hand.size()) + ")" + RESET;
    lines.push_back(header);

    const int cardW = 22;
    const int perRow = max(1, width / (cardW + 1));

    for (size_t start = 0; start < hand.size(); start += perRow)
    {
        size_t end = min(hand.size(), start + perRow);

        vector<vector<string>> cards;
        for (size_t i = start; i < end; i++)
        {
            const Card& card = hand[i];
            vector<string> c;
            int inner = cardW - 2;

            c.push_back("┌" + string(cardW - 2, '-') + "┐");
            string tag = "[" + card.getTypeString() + "]";
            c.push_back("│" + center(color + tag + RESET, inner) + "│");

            string cardName = card.getName();
            if ((int)cardName.size() > inner)
                cardName = cardName.substr(0, inner - 1) + ".";
            c.push_back("│" + center(BOLD + cardName + RESET, inner) + "│");

            if (card.isAttack() || card.isDefense() || card.isVersatile())
                c.push_back("│" + center("Val: " + to_string(card.getAttack()) + "  Boost: " + to_string(card.getBoost()), inner) + "│");
            else
                c.push_back("│" + center("Boost: " + to_string(card.getBoost()), inner) + "│");

            vector<string> effectLines = wrapText(card.geteffect(), inner - 1);
            for (int e = 0; e < 2; e++)
            {
                string txt = e < (int)effectLines.size() ? effectLines[e] : "";
                c.push_back("│ " + padVisible(DIM + txt + RESET, inner - 1) + "│");
            }
            c.push_back("│" + center("(" + to_string(i + 1) + ")", inner) + "│");
            c.push_back("└" + string(cardW - 2, '-') + "┘");

            cards.push_back(c);
        }

        size_t cardLines = cards.empty() ? 0 : cards[0].size();
        for (size_t li = 0; li < cardLines; li++)
        {
            string row;
            for (auto& c : cards) row += c[li] + " ";
            lines.push_back(row);
        }
    }

    if (hand.empty())
        lines.push_back(DIM + "(empty hand)" + RESET);

    return lines;
}

// ─────────────────────────────── render ───────────────────────────────

void GameTUI::render(Player& p1, Player& p2, Player* currentTurn, Bord& bord)
{
    Player& dracula  = (p1.getHero()->getName() == "Dracula") ? p1 : p2;
    Player& sherlock = (&dracula == &p1) ? p2 : p1;

    clearScreen();

    const int totalWidth = 110;
    cout << BOLD << CYAN << string(totalWidth, '=') << RESET << "\n";
    cout << center(BOLD + "UNMATCHED - " + dracula.getName() + " (Dracula) vs " + sherlock.getName() + " (Sherlock Holmes)" + RESET, totalWidth) << "\n";

    string turnName = currentTurn ? displayName(currentTurn->getHero()->getName()) + " (" + currentTurn->getName() + ")" : "?";
    string turnColor = (currentTurn == &dracula) ? RED : BLUE;
    cout << center(turnColor + BOLD + "Turn: " + turnName + RESET, totalWidth) << "\n";
    cout << BOLD << CYAN << string(totalWidth, '=') << RESET << "\n\n";

    vector<string> leftPanel  = buildHeroPanel(dracula,  currentTurn == &dracula,  RED,  30);
    vector<string> rightPanel = buildHeroPanel(sherlock, currentTurn == &sherlock, BLUE, 30);
    vector<string> mapPanel   = buildMapLines(bord, dracula, sherlock);

    printSideBySide(leftPanel, mapPanel, rightPanel);

    cout << "\n" << DIM << string(totalWidth, '-') << RESET << "\n\n";

    // Only show the hand of whoever's turn it is - not both players'.
    if (currentTurn != nullptr)
    {
        string handColor = (currentTurn == &dracula) ? RED : BLUE;
        for (auto& line : buildHandPanel(*currentTurn, handColor, totalWidth))
            cout << line << "\n";
    }

    cout << "\n" << DIM << string(totalWidth, '=') << RESET << "\n\n";
}