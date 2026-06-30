#include <iostream>
#include <string>
#include "Hero.hpp"

using namespace std;

Hero::Hero(): Character("", 0, 0, 0)
{
}

void Hero::showHeroes() const
{
    cout << "\n                               HEROES\n";
    cout << "┌────┬───────────┬────┬────────┬──────┬──────────┬─────────────────┐\n";
    cout << "│ No │ Name      │ HP │ Range  │ Move │ Sidekick │ Play Style      │\n";
    cout << "├────┼───────────┼────┼────────┼──────┼──────────┼─────────────────┤\n";
    cout << "│ 1  │ Sherlock  │ 16 │ Melee  │  2   │ Watson   │ Control         │\n";
    cout << "│ 2  │ Dracula   │ 13 │ Melee  │  2   │ Sisters  │ Aggressive      │\n";
    cout << "└────┴───────────┴────┴────────┴──────┴──────────┴─────────────────┘\n";
}

Hero::~Hero()
{
}
