#include <iostream>
#include <string>
using namespace std;

#include "Controller.hpp"
#include "Player.hpp"
#include "Bord.hpp"

int main()
{
	cout << "════════════════════════════════════════════════════════════════════\n";
    cout << "                 welcome to the game : (UNMATCHED)\n";
    cout << "════════════════════════════════════════════════════════════════════\n";

    string n;
    int a;

    Player player[2];
    
    cout << "First player, enter your name: ";
    getline(cin , n);
    player[0].setName(n);

    cout << player[0].getName() << ", enter your age: ";
    cin >> a;
    player[0].setAge(a);
    
    
    cout << "Second player, enter your name: ";
    getline(cin >> ws , n);
    player[1].setName(n);
    
    cout << player[1].getName() << ", enter your age: ";
    cin >> a;
    player[1].setAge(a);

	int first;
    int second;
	if(player[0].getAge() >= player[1].getAge()) 
    {
		first = 0;
        second = 1;
    }
	else
    {
		first = 1;
        second = 0;
    }


	int cha;
	cout << "\nCharacters:" << endl;
    cout << " 1.Dracula" << endl;
    cout << " 2.Sherlock" << endl;

    cout << player[first].getName() << ", choose your Character: ";
    cin >> cha;
    
    int pos[2];
    cout << player[first].getName() << ", choose your Character position(4 or 15): ";
    cin >> pos[0];

    while(pos[0] != 4 && pos[0] != 15)
    {
        cout << "Enter a valid option(4 or 15): ";
        cin >> pos[0];
    }
    
    if(pos[0] == 4)
    {
        pos[1] = 15;
    }
    else
    {
        pos[1] = 4;
    }


    if(cha == 1)
    {
	    player[first].chooseCharacter(1 , pos[0]);
        player[first].setCharacterName("Dracula");

        player[second].chooseCharacter(2 , pos[1]);
        player[second].setCharacterName("Shelock");
    }
	else 
    {
        player[first].chooseCharacter(2 , pos[0]);
        player[first].setCharacterName("Shelock");

        player[second].chooseCharacter(1 , pos[1]);
        player[second].setCharacterName("Dracula");
    }

    Bord bord;
    
        


  	return 0;
}
  