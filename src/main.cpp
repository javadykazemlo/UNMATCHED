#include <iostream>
#include <string>
using namespace std;

#include "Controller.hpp"
#include "Player.hpp"

int main()
{
	cout << "════════════════════════════════════════════════════════════════════ \n";
    cout << "                 welcome to the game : (UNMATCHED)\n";
    cout << "════════════════════════════════════════════════════════════════════  \n";

    string n;
    int a;

    Player player1;
    
    cout << "First player, enter your name: ";
    getline(cin , n);
    player1.setName(n);

    cout << player1.getName() << ", enter your age: ";
    cin >> a;
    player1.setAge(a);
    
    
    Player player2;
    
    cout << "Second player, enter your name: ";
    getline(cin >> ws , n);
    player2.setName(n);
    
    cout << player2.getName() << ", enter your age: ";
    cin >> a;
    player2.setAge(a);


  	return 0;
}
  