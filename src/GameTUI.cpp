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

// Hand-computed BFS-layered layout of the 32 board spaces (see the real
// adjacency graph in Bord.cpp). Column = distance from the Dracula start
// space (4), row = position within that "ring" so the graph prints as a
// readable left-to-right diagram instead of a jumble of numbers.
GameTUI::NodePos GameTUI::nodePos(int id)
{
    static const NodePos table[32] = {
        /*0*/  {2,1}, /*1*/  {1,2}, /*2*/  {3,2}, /*3*/  {2,2},
        /*4*/  {0,3}, /*5*/  {1,3}, /*6*/  {1,4}, /*7*/  {2,3},
        /*8*/  {2,4}, /*9*/  {3,3}, /*10*/ {4,2}, /*11*/ {4,3},
        /*12*/ {5,0}, /*13*/ {5,1}, /*14*/ {6,0}, /*15*/ {5,2},
        /*16*/ {4,4}, /*17*/ {5,3}, /*18*/ {6,1}, /*19*/ {5,4},
        /*20*/ {2,5}, /*21*/ {3,4}, /*22*/ {4,5}, /*23*/ {5,5},
        /*24*/ {6,2}, /*25*/ {7,3}, /*26*/ {6,3}, /*27*/ {6,4},
        /*28*/ {5,6}, /*29*/ {5,7}, /*30*/ {6,5}, /*31*/ {6,6},
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
            glyph = isDraculaSide ? "D " : "S ";
        else if (!name.empty())
            glyph = string(1, toupper(name.back())) + " "; // last word initial for sidekicks

        // nicer sidekick initials: use first letter of last name token
        {
            size_t sp = name.find_last_of(' ');
            char letter = sp == string::npos ? name[0] : name[sp + 1];
            glyph = string(1, (char)toupper(letter)) + " ";
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

vector<string> GameTUI::buildMapLines(Bord& bord, Player& dracula, Player& sherlock)
{
    const int COLS = 8, ROWS = 8;
    int grid[COLS][ROWS];
    for (int c = 0; c < COLS; c++)
        for (int r = 0; r < ROWS; r++)
            grid[c][r] = -1;

    for (int id = 0; id < 32; id++)
    {
        NodePos p = nodePos(id);
        grid[p.col][p.row] = id;
    }

    auto adjacent = [&](int a, int b) {
        if (a < 0 || b < 0) return false;
        vector<int> adj = bord.getposAdjacent(a);
        return find(adj.begin(), adj.end(), b) != adj.end();
    };

    vector<string> lines;
    lines.push_back(BOLD + CYAN + "MAP (Graph)" + RESET);
    lines.push_back("");

    for (int r = 0; r < ROWS; r++)
    {
        // node row
        string nodeLine;
        for (int c = 0; c < COLS; c++)
        {
            int id = grid[c][r];
            string label = id == -1 ? "  " : nodeGlyph(bord, id, dracula, sherlock);
            nodeLine += label;
            if (c != COLS - 1)
                nodeLine += adjacent(id, grid[c + 1][r]) ? GREY + "──" + RESET : "  ";
        }
        lines.push_back(nodeLine);

        if (r != ROWS - 1)
        {
            // connector row (vertical + diagonal edges)
            string connLine;
            for (int c = 0; c < COLS; c++)
            {
                int here = grid[c][r];
                int down = grid[c][r + 1];
                connLine += adjacent(here, down) ? (GREY + "|" + RESET) : " ";
                connLine += " ";

                if (c != COLS - 1)
                {
                    int downRight = grid[c + 1][r + 1];
                    int hereRight = grid[c + 1][r];
                    bool diagDR = adjacent(here, downRight);
                    bool diagDL = adjacent(hereRight, down);
                    if (diagDR)      connLine += GREY + "\\" + RESET;
                    else if (diagDL) connLine += GREY + "/" + RESET;
                    else             connLine += " ";
                }
            }
            lines.push_back(connLine);
        }
    }

    lines.push_back("");
    lines.push_back(DIM + "Legend:" + RESET);
    lines.push_back(RED + BOLD + "D" + RESET + DIM + " Dracula   " + RESET +
                     BLUE + BOLD + "S" + RESET + DIM + " Sherlock" + RESET);
    lines.push_back(DIM + "Letters = sidekicks   " + RESET + YELLOW + "##" + RESET +
                     DIM + " = secret passage" + RESET);
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

    const int totalWidth = 100;
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

    for (auto& line : buildHandPanel(dracula, RED, totalWidth))
        cout << line << "\n";
    cout << "\n";
    for (auto& line : buildHandPanel(sherlock, BLUE, totalWidth))
        cout << line << "\n";

    cout << "\n" << DIM << string(totalWidth, '=') << RESET << "\n\n";
}
