#pragma once
#include <string>
#include <vector>

class Player;
class Bord;
class Character;
class Card;

// ─────────────────────────────────────────────────────────────────────────
//  GameTUI
//
//  Draws the "fixed" top part of the screen (title, whose turn it is, the
//  map, both heroes' stats and both players' hands) exactly like a small
//  dashboard. Every time render() is called the terminal is cleared and the
//  whole dashboard is redrawn from scratch, so from the player's point of
//  view it always looks like the same picture just refreshing in place.
//  Everything else (menus, prompts, "Enter command:", action log text, ...)
//  keeps working exactly like before: plain std::cin / std::cout under the
//  dashboard.
// ─────────────────────────────────────────────────────────────────────────
class GameTUI
{
public:
    // Draws the whole dashboard. p1/p2 can be passed in any order, the
    // hero name is used to figure out which one is Dracula and which one
    // is Sherlock so the layout is always Dracula-left / Sherlock-right.
    static void showWelcome();
    static void render(Player& p1, Player& p2, Player* currentTurn, Bord& bord);
    
    static std::vector<std::string> buildHandPanel(const std::vector<Card>& hand, const std::string& ownerName, const std::string& color, int width);
    static const std::string RED, BLUE, GREEN, YELLOW, CYAN, MAGENTA, GREY;
    
private:
    struct NodePos { int col; int row; };

    static void clearScreen();
    static int  visibleLength(const std::string& s);
    static std::string padVisible(const std::string& s, int width);
    static std::string center(const std::string& s, int width);

    static const std::string RESET, BOLD, DIM;

    static NodePos nodePos(int id);
    static bool isSecretPassage(int id);
    static std::string nodeGlyph(Bord& bord, int id, Player& dracula, Player& sherlock);

    static std::vector<std::string> buildMapLines(Bord& bord, Player& dracula, Player& sherlock);
    static std::vector<std::string> buildHeroPanel(Player& player, bool isTurn, const std::string& color, int width);

    static void printSideBySide(const std::vector<std::string>& left,
                                 const std::vector<std::string>& mid,
                                 const std::vector<std::string>& right);

    static std::vector<std::string> wrapText(const std::string& text, int width);
};
