#include "GameTUI.hpp"
#include <iostream>
#include <limits>
#include <algorithm>
#include <iomanip>

using namespace std;

GameTUI::GameTUI()
{
    initializeGrid();
    firstPlayer = 0;
    secondPlayer = 1;
    choice1 = 0;
    choice2 = 0;
}

void GameTUI::initializeGrid()
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            grid[i][j] = ' ';
        }
    }
}

void GameTUI::displaySeparator()
{
    cout << "============================================================\n";
}

void GameTUI::displayHeader(const string& title)
{
    displaySeparator();
    cout << "                 " << title << "\n";
    displaySeparator();
}

void GameTUI::displayBoard()
{
    cout << "\n";
    cout << "    ";
    for (int j = 0; j < 8; j++) {
        cout << " " << j + 1 << "  ";
    }
    cout << "\n";
    
    cout << "   +";
    for (int j = 0; j < 8; j++) {
        cout << "---+";
    }
    cout << "\n";
    
    for (int i = 0; i < 4; i++) {
        cout << " " << i + 1 << " |";
        for (int j = 0; j < 8; j++) {
            if (grid[i][j] == ' ') {
                cout << "   |";
            } else {
                cout << " " << grid[i][j] << " |";
            }
        }
        cout << "\n";
        
        cout << "   +";
        for (int j = 0; j < 8; j++) {
            cout << "---+";
        }
        cout << "\n";
    }
    cout << "\n";
    cout << "Legend: D = Dracula, S = Sherlock, s = Sister, W = Watson\n";
}

int GameTUI::getInt()
{
    string line;
    int x;
    while (true) {
        getline(cin, line);
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\rf\v") + 1);
        if (line.empty()) {
            cout << "Please enter a number: ";
            continue;
        }
        try {
            x = stoi(line);
            return x;
        }
        catch (const exception& e) {
            cout << "Invalid input. Please enter a number: ";
        }
    }
}

int GameTUI::getChoice(const vector<int>& valid)
{
    while (true) {
        int x = getInt();
        for (int i : valid) {
            if (i == x) return x;
        }
        cout << "Invalid choice. Try again: ";
    }
}

pair<int, int> GameTUI::getCoordinates(int pos)
{
    int row = (pos - 1) / 8;
    int col = (pos - 1) % 8;
    return {row, col};
}

void GameTUI::run()
{
    cout << "\n";
    displayHeader("Welcome to (UNMATCHED)");
    
    // Get player info
    string name;
    int age;
    
    cout << "First player, enter your name: ";
    getline(cin, name);
    players[0].setName(name);
    
    cout << players[0].getName() << ", enter your age: ";
    age = getInt();
    players[0].setAge(age);
    
    cout << "\nSecond player, enter your name: ";
    getline(cin >> ws, name);
    players[1].setName(name);
    
    cout << players[1].getName() << ", enter your age: ";
    age = getInt();
    players[1].setAge(age);
    
    cout << "\n";
    
    // Determine turn order (younger player goes first)
    if (players[0].getAge() <= players[1].getAge()) {
        firstPlayer = 0;
        secondPlayer = 1;
    } else {
        firstPlayer = 1;
        secondPlayer = 0;
    }
    
    displayHeader("Turn Order");
    cout << players[firstPlayer].getName() << " (younger) plays first.\n";
    cout << players[secondPlayer].getName() << " plays second.\n\n";
    
    // Choose characters
    displayHeader("Choose Character");
    cout << "Characters:\n";
    cout << " 1. Dracula  ->  Symbol: D\n";
    cout << " 2. Sherlock ->  Symbol: S\n";
    
    cout << "\n" << players[firstPlayer].getName() << ", choose your character (1 or 2): ";
    choice1 = getChoice({1, 2});
    
    choice2 = (choice1 == 1) ? 2 : 1;
    
    string char1 = (choice1 == 1) ? "Dracula" : "Sherlock";
    string char2 = (choice2 == 1) ? "Dracula" : "Sherlock";
    
    cout << players[firstPlayer].getName() << " chose " << char1 << ".\n";
    cout << players[secondPlayer].getName() << " gets " << char2 << ".\n\n";
    
    // Choose positions
    displayHeader("Choose Position");
    cout << "Valid positions: 4 (left) or 15 (right)\n";
    cout << "Board: rows 1-4, columns 1-8\n";
    cout << "Position 4 = row 1, col 4\n";
    cout << "Position 15 = row 2, col 7\n\n";
    
    int pos1, pos2;
    
    cout << players[firstPlayer].getName() << ", choose your position (4 or 15): ";
    pos1 = getChoice({4, 15});
    pos2 = (pos1 == 4) ? 15 : 4;
    
    cout << players[secondPlayer].getName() << ", your character goes to position " << pos2 << ".\n\n";
    
    // Place main characters
    auto [row1, col1] = getCoordinates(pos1);
    auto [row2, col2] = getCoordinates(pos2);
    
    char symbol1 = (choice1 == 1) ? 'D' : 'S';
    char symbol2 = (choice2 == 1) ? 'D' : 'S';
    
    grid[row1][col1] = symbol1;
    grid[row2][col2] = symbol2;
    
    // Place sidekicks
    displayHeader("Place Sidekicks");
    
    if (choice1 == 1) {
        cout << players[firstPlayer].getName() << " (Dracula): 3 Sisters (s) placed.\n";
        for (int i = 1; i <= 3; i++) {
            int pos = pos1 + i;
            if (pos <= 32) {
                auto [row, col] = getCoordinates(pos);
                if (grid[row][col] == ' ') {
                    grid[row][col] = 's';
                }
            }
        }
    } else {
        cout << players[firstPlayer].getName() << " (Sherlock): 1 Watson (W) placed.\n";
        int pos = pos1 + 1;
        if (pos <= 32) {
            auto [row, col] = getCoordinates(pos);
            if (grid[row][col] == ' ') {
                grid[row][col] = 'W';
            }
        }
    }
    
    if (choice2 == 1) {
        cout << players[secondPlayer].getName() << " (Dracula): 3 Sisters (s) placed.\n";
        for (int i = 1; i <= 3; i++) {
            int pos = pos2 + i;
            if (pos <= 32) {
                auto [row, col] = getCoordinates(pos);
                if (grid[row][col] == ' ') {
                    grid[row][col] = 's';
                }
            }
        }
    } else {
        cout << players[secondPlayer].getName() << " (Sherlock): 1 Watson (W) placed.\n";
        int pos = pos2 + 1;
        if (pos <= 32) {
            auto [row, col] = getCoordinates(pos);
            if (grid[row][col] == ' ') {
                grid[row][col] = 'W';
            }
        }
    }
    
    cout << "\n";
    
    // Show final board
    displayHeader("Final Board");
    displayBoard();
    
    // Show final info
    displayHeader("Player Information");
    cout << "  " << players[firstPlayer].getName() << " (" << players[firstPlayer].getAge() << " years) - ";
    cout << (choice1 == 1 ? "Dracula" : "Sherlock") << "\n";
    cout << "  " << players[secondPlayer].getName() << " (" << players[secondPlayer].getAge() << " years) - ";
    cout << (choice2 == 1 ? "Dracula" : "Sherlock") << "\n";
    displaySeparator();
}

GameTUI::~GameTUI()
{
}