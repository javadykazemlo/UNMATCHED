#include <iostream>
#include <string>
#include <vector>
#include "Sister.hpp"

using namespace std;

Sister::Sister(string name): Character(name , 1 , 2 , 0)
{
}


void Sister::showSister() const
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

void Sister::ability()
{
}

Sister::~Sister()
{
}