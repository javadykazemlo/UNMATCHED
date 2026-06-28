#include "Bord.hpp"
#include <iostream>
#include <vector>

using namespace std;

Bord::Bord()
{
    spaces[0].adjacent = {1, 2};                  //a
    spaces[1].adjacent = {0, 4};                  //a
    spaces[2].adjacent = {0, 3, 21};              //a
    spaces[3].adjacent = {2, 5};                  //a
    spaces[4].adjacent = {1, 5, 6};               //ab
    spaces[5].adjacent = {3, 4, 20};              //af
    spaces[6].adjacent = {4, 7, 8};               //b
    spaces[7].adjacent = {6, 8, 9};               //b
    spaces[8].adjacent = {6, 7, 9};               //b
    spaces[9].adjacent = {7, 8, 10, 11, 16};      //bce
    spaces[10].adjacent = {9, 11, 12};            //c
    spaces[11].adjacent = {9, 10};                //c
    spaces[12].adjacent = {10};                   //c
    spaces[13].adjacent = {11, 14, 16};           //d
    spaces[14].adjacent = {13, 15, 31};           //d
    spaces[15].adjacent = {14, 16};               //d
    spaces[16].adjacent = {9, 13, 15, 17, 28, 29};//de
    spaces[17].adjacent = {16, 18};               //e
    spaces[18].adjacent = {17, 19, 26};           //e
    spaces[19].adjacent = {18, 22};               //ef
    spaces[20].adjacent = {5, 21};                //f
    spaces[21].adjacent = {2, 20, 22};            //f
    spaces[22].adjacent = {19, 21, 23};           //fg
    spaces[23].adjacent = {22, 24};               //g
    spaces[24].adjacent = {23, 25};               //g 
    spaces[25].adjacent = {24, 26, 27};           //g
    spaces[26].adjacent = {18, 25, 27, 28};       //ge
    spaces[27].adjacent = {25, 26, 28};           //g
    spaces[28].adjacent = {16, 26, 27, 29};       //g
    spaces[29].adjacent = {16, 28, 30, 31};       //g
    spaces[30].adjacent = {29, 31};               //g
    spaces[31].adjacent = {14, 29, 30};           //gd


    spaces[0].zone = {1};
    spaces[1].zone = {1};
    spaces[2].zone = {1};
    spaces[3].zone = {1};
    spaces[4].zone = {1, 2};
    spaces[5].zone = {1, 6};
    spaces[6].zone = {2};
    spaces[7].zone = {2};
    spaces[8].zone = {2};
    spaces[9].zone = {2, 3, 5};
    spaces[10].zone = {3};
    spaces[11].zone = {3};
    spaces[12].zone = {3};
    spaces[13].zone = {4};
    spaces[14].zone = {4};
    spaces[15].zone = {4};
    spaces[16].zone = {4, 5};
    spaces[17].zone = {5};
    spaces[18].zone = {5};
    spaces[19].zone = {5, 6};
    spaces[20].zone = {6};
    spaces[21].zone = {6};
    spaces[22].zone = {6, 7};
    spaces[23].zone = {7};
    spaces[24].zone = {7}; 
    spaces[25].zone = {7};
    spaces[26].zone = {5, 7};
    spaces[27].zone = {7};
    spaces[28].zone = {7};
    spaces[29].zone = {7};
    spaces[30].zone = {7};
    spaces[31].zone = {4, 7};

}
    
Bord::~Bord()
{

}