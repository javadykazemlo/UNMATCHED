#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "cards/Card.hpp"


using namespace std;

void Card::applyEffect(Player* pl,Player* enemyPl, Bord*bo)
{


  if (name == "Administer Aid")
    {

        Character* holmes = pl->getHero();
        Character* watson = pl->getsidekick(1);
        vector<int> adjacent = bo->getAdjacent(holmes);
        vector<int> emptyAdjacent;


        for (auto pos : adjacent)
        {
            if (bo->isEmpty(pos))
            {
                emptyAdjacent.push_back(pos);
            }
            
        }
        if (!emptyAdjacent.empty())
        {
            for (int i = 0; i < 32; i++)
            {
                if (bo->getCharacterName(i) == watson)
                {
                    bo->deletCharacter(i);
                    break;
                }
            }
        }

        for (int i = 0; i <emptyAdjacent.size() ; i++)
        {
            if(bo->isEmpty(emptyAdjacent[i]))
            {
                bo->addCharacter(emptyAdjacent[i],watson);      
            }
            
        }
        
          
        holmes->heal(1);
        pl->getDeck()->cardHandSH();
        cout << "Holmes healed 1 HP and 1 card drawn!\n";
        

    }
    
    else if (name == "Confirm Suspicion")
    {
        
        cout << "Enter a number for choose card ";
    }
    
    else if (name == "Counter Punch")
    { 
        Character* holmes = pl->getHero();
        Character* enemyHero = enemyPl->getHero();
        Character* enemySidekick = enemyPl->getsidekick(1);

        vector<int> adjacent = bo->getAdjacent(holmes);
        bool damaged = false;

        for (int pos : adjacent)
        {
            if (!bo->isEmpty(pos))
            {
                Character* target = bo->getCharacterName(pos);
                
                if (target == enemyHero || target == enemySidekick)
                {
                    target->takeDamage(0, 2);
                    cout << "2 damage dealt to " << target->getName() << "!\n";
                    damaged = true;
                    break;  
                }
            }
        }
        
        if (!damaged)
        {
            cout << "No adjacent enemy fighter!\n";
        }
        

    }
    
    else if (name == "Deduce Strategy")
    {
        
    }
    
    else if (name == "Education Never Ends")
    {
        
    }
    
    else if (name == "Elementary")
    {
        
    }
    
    else if (name == "Eliminate the Impossible")
    {
        
    }
    
    else if (name == "Feint")
    {
        
    }
    
    else if (name == "Fixed Point in a Changing Age")
    {
        
    }
    
    else if (name == "Master of Disguise")
    {
        
    }
    
    else if (name == "The Game Is Afoot")
    {
        
    }
    
    else if (name == "Service Revolver")
    {
        
    }
    
    else if (name == "Study Methods")
    {
        
    }

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    
    else if (name == "Feeding Frenzy")
    {
        
    }
    
    else if (name == "Mistform")
    {
        
    }
    
    else if (name == "Ambush")
    {
        
    }
    
    else if (name == "Baptism of Blood")
    {
        
    }
    
    else if (name == "Beastform")
    {
        
    }
    
    else if (name == "Dash")
    {
        
    }
    
    else if (name == "Exploit")
    {
        
    }
    
    else if (name == "Look Into My Eyes")
    {
        
    }
    
    else if (name == "Prey Upon")
    {
        
    }
    
    else if (name == "Ravening Seduction")
    {
        
    }
    
    else if (name == "Thirst for Sustenance")
    {
        
    }
    
    else
    {
        cout << "No special effect for this card.\n";
    }


}    


