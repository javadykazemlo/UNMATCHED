#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

#include "Controller.hpp"
#include "Player.hpp"
#include "Bord.hpp"

int main()
{
	cout << "════════════════════════════════════════════════════════════════════\n";
    cout << "                 welcome to the game : (UNMATCHED)\n";
    cout << "════════════════════════════════════════════════════════════════════\n";
    srand(static_cast<unsigned int>(time(0))) ;
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
	if(player[0].getAge() <= player[1].getAge()) 
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
    
    while(cha != 1 && cha != 2)
    {
        cout << "Enter a valid option(4 or 15): ";
        cin >> cha;
    }

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
    
    Bord bord;
    Controller k;
    cout << "-----------------------------------------------";
    if(cha == 1)
    {
        player[first].chooseCharacter(1);
        bord.addCharacter( pos[0] , "Dracula" );
        k.plaseSidekicks(1 , bord);
        
        player[second].chooseCharacter(2);
        bord.addCharacter( pos[1] , "Shelock" );
        k.plaseSidekicks(2 , bord);
    }
	else
    {
        player[first].chooseCharacter(2);
        bord.addCharacter( pos[0] , "Shelock" );
        k.plaseSidekicks(2 , bord);
        
        player[second].chooseCharacter(1);
        bord.addCharacter( pos[1] , "Dracula" );
        k.plaseSidekicks(1 , bord);
    }



    
    cout << player[first].getName() << " chose " << player[first].getHero()->getName() << " and starts at spaces " << pos[0] << ".\n"; 
    cout << player[second].getName() << " chose " << player[second].getHero()->getName() << " and starts at spaces " << pos[1] << ".\n"; 
    
    
        


  	return 0;
}
  