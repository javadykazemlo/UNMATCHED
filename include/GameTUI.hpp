#pragma once
#include <string>
#include <vector>

class Player;
class Bord;
class Character;
class Card;

class GameTUI
{
public:
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