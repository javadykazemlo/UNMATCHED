#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "cards/Card.hpp"


using namespace std;


Card::Card(string nam , owner own , CardType typ , int att , timing tim , int bos , string efec)
{
    this-> Name = nam;
    this-> Owner = own;
    this-> Type = typ;
    this-> Attack = att; 
    this-> Timing = tim; 
    this-> Boost = bos; 
    this->effect = efec;
}
    
void Card::Effect()
{

}

bool Card::isHero() const
{
    return Owner == owner::Hero;
}

bool Card::issideKick() const
{
    return Owner == owner::SideKick;
}

bool Card::isAnyowner() const
{
    return Owner == owner::Any;
}


bool Card::isAttack() const
{
    return Type == CardType::Attack;
}

bool Card::isDefense() const
{
    return Type == CardType::Defense;
}

bool Card::isVersatile() const
{
    return Type == CardType::Versatile;
}

bool Card::isScheme() const
{
    return Type == CardType::Scheme;
}


bool Card::isNonetiming() const
{
    return Timing == timing::None;
}

bool Card::isBeforeCombat() const
{
    return Timing == timing::BeforeCombat;
}

bool Card::isDuringCombat() const
{
    return Timing == timing::DuringCombat;
}

bool Card::isAfterCombat() const
{
    return Timing == timing::AfterCombat;
}

std::string Card::getOwnerString() const
{
    switch (Owner)
    {
        case owner::Hero:
            return "Hero";

        case owner::SideKick:
            return "SideKick";

        case owner::Any:
            return "Any";
    }

    return "Unknown";
}

string Card::getTypeString() const
{
    switch (Type)
    {
        case CardType::Attack:
            return "Attack";

        case CardType::Defense:
            return "Defense";

        case CardType::Versatile:
            return "Versatile";

        case CardType::Scheme:
            return "Scheme";
    }

    return "Unknown";
}

std::string Card::getTimingString() const
{
    switch (Timing)
    {
        case timing::None:
            return "None";

        case timing::BeforeCombat:
            return "Before Combat";

        case timing::DuringCombat:
            return "During Combat";

        case timing::AfterCombat:
            return "After Combat";
    }

    return "Unknown";
}

string Card::getName() const
{
    return Name;
}

owner Card::getOwner() const 
{
    return Owner;
}

CardType Card::getType() const
{
    return Type;
}

int Card::getAttack() const 
{
    return Attack;
}

timing Card::getTiming() const
{
    return Timing;
}

int Card::getBoost() const 
{
    return Boost;
}

string Card::geteffect() const
{
    return effect;
}


Card::~Card()
{
}


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
        Controller k ;
        if (k.get_DraculaWon())
        {
            cout << "Dracula won! Opponent draws 1 card.\n";
            enemyPl->getDeck()->cardHandSH();
        }
        else if (k.get_SherlockWon())
        {
            cout << "Sherlock won! Opponent draws 1 card.\n";
            enemyPl->getDeck()->cardHandSH();
        }
        else
        {
            cout << "You lost or tied! You draw 2 cards.\n";
            pl->getDeck()->cardHandSH();
            pl->getDeck()->cardHandSH();
        }
        
    }
    
    else if (name == "Elementary")
    {
        /*
        cout << "Guess the attack value: ";
        int guess;
        cin >> guess;
        Controller k ;
        if (برای اون بولین هستت)
        {
            cout <<"You guessed -> succeessful";
        }
        else
        {
            cout <<"You guessed -> failed";

        }
        */
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


