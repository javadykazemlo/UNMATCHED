#pragma once
#include <string>
#include <vector>
#include "core/Player.hpp"
#include "core/Bord.hpp"

class GameTUI
{
private:
    char grid[4][8];
    Player players[2];
    Bord board;
    int firstPlayer;
    int secondPlayer;
    int choice1;
    int choice2;

    void initializeGrid();
    void displayBoard();
    void displayHeader(const std::string& title);
    void displaySeparator();
    int getInt();
    int getChoice(const std::vector<int>& valid);
    std::pair<int, int> getCoordinates(int pos);

public:
    GameTUI();
    void run();
    ~GameTUI();
};