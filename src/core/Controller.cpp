#include <iostream>
#include <string>
#include "Controller.hpp"

using namespace std;

Controller::Controller()
{

}

void Controller::plaseSidekicks(int cha)
{
    vector<int> zon;
    vector<int> space;
    int s;
    switch(cha)
    {
        case 1:
            zon = bord.getZone("Dracula");
            space = bord.getCellThisZone(zon);
            for(int i = 0 ; i < 3 ; i++)
            {
                cout << endl << "Available space: " << endl;
                for(int j = 0 ; j < zon.size()  ; j++)
                {
                    if(!bord.getSpaceStatus(i))
                        cout << "   " << zon[j];  
                }
                cout << "\nwhere do you want to place the " << i + 1 << "sister: ";
                cin >> s;
                for(int j = 0 ; j < zon.size()  ; j++)
                {
                    if(!bord.getSpaceStatus(s) && s == zon[j])
                        bord.addCharacter( s , "sister" );
                }

            }


            break;

        case 2:
            zon = bord.getZone("sherlock");
            space = bord.getCellThisZone(zon);

            cout << endl << "Available space: " << endl;
            for(int j = 0 ; j < zon.size()  ; j++)
            {
                if(!bord.getSpaceStatus(j))
                    cout << "   " << zon[j];  
            }
            cout << "\nwhere do you want to place the Dr_watson: ";
            cin >> s;
            for(int j = 0 ; j < zon.size()  ; j++)
            {
                if(!bord.getSpaceStatus(s) && s == zon[j])
                    bord.addCharacter( s , "Dr_watson" );
            }
            break;
    }

}

Controller::~Controller()
{

}