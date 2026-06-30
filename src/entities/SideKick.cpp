#include <iostream>
#include <string>
#include "SideKick.hpp"

using namespace std;

SideKick::SideKick(): Character("", 0, 0, 0)
{
}

void SideKick::showSideKicks() const
{
    cout << "\n                     SIDEKICKS\n";
    cout << "┌────┬────────────────┬───────────┬────┬────────┬──────┐\n";
    cout << "│ No │ Name           │ Hero      │ HP │ Attack │ Move │\n";
    cout << "├────┼────────────────┼───────────┼────┼────────┼──────┤\n";
    cout << "│ 1  │ Dr. Watson     │ Sherlock  │  8 │ Melee  │  2   │\n";
    cout << "│ 2  │ Sister Agatha  │ Dracula   │  1 │ Melee  │  2   │\n";
    cout << "│ 3  │ Sister Beatrix │ Dracula   │  1 │ Melee  │  2   │\n";
    cout << "│ 4  │ Sister Carmilla│ Dracula   │  1 │ Melee  │  2   │\n";
    cout << "└────┴────────────────┴───────────┴────┴────────┴──────┘\n";
}

void SideKick::ability()
{
}

SideKick::~SideKick()
{
}