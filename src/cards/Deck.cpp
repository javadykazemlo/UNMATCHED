#include <iostream>
#include <string>
#include <iomanip>
#include "cards/Deck.hpp"

using namespace std;
Deck::Deck(std::vector<Card>& sh, std::vector<Card>& dr): cardsSH(sh), cardsDR(dr)
{
    
}

void Deck::SherlockDeck() 
{

    cardsSH.clear();

    for (int i = 0; i < 2; i++) 
    {
        cardsSH.push_back(
            {
            "Administer Aid",
            "Watson",
            "Scheme",
            0,
            0,
            "",
            2,
            1,
            false,
            "Place Watson adjacent to Holmes. Heal Holmes by 1 and draw 1 card."
        });
    }

    for (int i = 0; i < 3; i++) 
    {
        cardsSH.push_back(
            {
            "Confirm Suspicion",
            "Sherlock Holmes",
            "Scheme",
            0,
            0,
            "",
            1,
            1,
            true,
            "Name an attack or defense value. Your opponent must reveal and discard a matching card. The opposing hero suffers damage equal to that card's boost value. Otherwise they reveal their hand."
        });
    }

    for (int i = 0; i < 3; i++) 
    {
        cardsSH.push_back(
            {
            "Counter Punch",
            "Sherlock Holmes",
            "Versatile",
            3,
            3,
            "After Combat",
            1,
            1,
            false,
            "If Holmes is adjacent to the opposing fighter, deal 2 damage."
        });
    }

    for (int i = 0; i < 3; i++) 
    {
        cardsSH.push_back(
            {
            "Deduce Strategy",
            "Sherlock Holmes",
            "Versatile",
            3,
            3,
            "During Combat",
            1,
            1,
            false,
            "You may change your opponent's printed combat value to its boost value."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsSH.push_back(
            {
            "Education Never Ends",
            "Any Fighter",
            "Versatile",
            3,
            3,
            "After Combat",
            1,
            1,
            false,
            "If you won, your opponent draws 1 card. Otherwise draw 2 cards."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsSH.push_back(
            {
            "Elementary",
            "Sherlock Holmes",
            "Defense",
            0,
            3,
            "During Combat",
            3,
            1,
            true,
            "Guess the attack value. If correct, ignore the opponent's attack and cancel all effects."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsSH.push_back(
            {
            "Eliminate the Impossible",
            "Sherlock Holmes",
            "Scheme",
            0,
            0,
            "",
            2,
            1,
            false,
            "Look at your opponent's hand and discard one card."
        });
    }

    for (int i = 0; i < 3; i++) 
    {
        cardsSH.push_back(
            {
            "Feint",
            "Any Fighter",
            "Versatile",
            2,
            2,
            "Before Combat",
            1,
            1,
            false,
            "Cancel all effects on your opponent's card."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsSH.push_back(
            {
            "Fixed Point in a Changing Age",
            "Watson",
            "Versatile",
            3,
            3,
            "After Combat",
            1,
            1,
            false,
            "If Watson is adjacent to Holmes, both recover 1 health."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsSH.push_back(
            {
            "Master of Disguise",
            "Sherlock Holmes",
            "Scheme",
            0,
            0,
            "",
            2,
            1,
            false,
            "Swap Holmes with an opposing fighter and deal 1 damage."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsSH.push_back(
            {
            "The Game Is Afoot",
            "Sherlock Holmes",
            "Attack",
            5,
            0,
            "After Combat",
            2,
            1,
            false,
            "Move Holmes up to 3 spaces."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsSH.push_back(
            {
            "Service Revolver",
            "Watson",
            "Attack",
            5,
            0,
            "",
            3,
            1,
            false,
            "No additional effect."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsSH.push_back(
            {
            "Study Methods",
            "Any Fighter",
            "Versatile",
            3,
            3,
            "After Combat",
            2,
            1,
            false,
            "If you won the combat, you may look at your opponent's hand."
        });
    }
}

void Deck::DraculaDeck() 
{

    cardsDR.clear();

    for (int i = 0; i < 2; i++) 
    {
        cardsDR.push_back(
            {
            "Feeding Frenzy",
            "Dracula",
            "Attack",
            2,
            0,
            "During Combat",
            3,
            1,
            false,
            "This attack gains +1 for each Sister in the same zone as the opposing fighter."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsDR.push_back(
            {
            "Mistform",
            "Dracula",
            "Scheme",
            0,
            0,
            "",
            2,
            1,
            false,
            "Place Dracula in any space and gain 1 action."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsDR.push_back(
            {
            "Ambush",
            "Any Fighter",
            "Attack",
            2,
            0,
            "During Combat",
            3,
            1,
            false,
            "Opponent discards a random card. Add its boost value to this attack."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsDR.push_back(
            {
            "Baptism of Blood",
            "Dracula",
            "Scheme",
            0,
            0,
            "",
            2,
            1,
            false,
            "Recover 2 health. If a Sister is defeated, return her to any Dracula space."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsDR.push_back(
            {
            "Beastform",
            "Dracula",
            "Attack",
            6,
            0,
            "During Combat",
            4,
            1,
            false,
            "Discard any number of cards. Gain +1 attack for each discarded card."
        });
    }

    for (int i = 0; i < 3; i++) 
    {
        cardsDR.push_back(
            {
            "Dash",
            "Any Fighter",
            "Versatile",
            3,
            3,
            "After Combat",
            1,
            1,
            false,
            "Move your fighter up to 3 spaces."
        });
    }

    for (int i = 0; i < 3; i++) 
    {
        cardsDR.push_back(
            {
            "Exploit",
            "Any Fighter",
            "Versatile",
            4,
            4,
            "After Combat",
            1,
            1,
            false,
            "Draw 1 card."
        });
    }

    for (int i = 0; i < 3; i++) 
    {
        cardsDR.push_back(
            {
            "Look Into My Eyes",
            "Dracula",
            "Defense",
            0,
            1,
            "During Combat",
            2,
            1,
            false,
            "Add the boost value of your opponent's attack card to this defense."
        });
    }

    for (int i = 0; i < 2; i++) 
    {
        cardsDR.push_back(
            {
            "Prey Upon",
            "Dracula",
            "Scheme",
            0,
            0,
            "",
            4,
            1,
            false,
            "Deal 1 damage to all adjacent opposing fighters. Dracula heals 1 for each damage dealt."
        });
    }

    for (int i = 0; i < 3; i++) 
    {
        cardsDR.push_back(
            {
            "Ravening Seduction",
            "Sister",
            "Scheme",
            0,
            0,
            "",
            2,
            1,
            false,
            "Move any fighter up to 2 spaces. Then deal 1 damage for each adjacent Sister."
        });
    }

    for (int i = 0; i < 3; i++) 
    {
        cardsDR.push_back(
            {
            "Thirst for Sustenance",
            "Sister",
            "Attack",
            3,
            0,
            "After Combat",
            3,
            1,
            false,
            "If you won the combat, place Dracula adjacent to the opposing fighter."
        });
    }

    for (int i = 0; i < 3; i++) 
    {
        cardsDR.push_back(
            {
            "Feint",
            "Any Fighter",
            "Versatile",
            2,
            2,
            "Before Combat",
            2,
            1,
            false,
            "Cancel all effects on your opponent's card."
        });
    }
}




    void Deck::showCard(const Card& card) const 
    {
        cout << "┌────────────────────────────────────────────────────┐\n";
        cout << "│ " << left << setw(50) << card.name << "│\n";
        cout << "├────────────────────────────────────────────────────┤\n";
        cout << "│ " << left << setw(50) << (card.owner + " · " + card.type) << "│\n";
        cout << "├────────────────────────────────────────────────────┤\n";
    
    if (card.type == "Attack") 
    {
        cout << "│ " << left << setw(50) << ("Attack: " + to_string(card.attack)) << "│\n";
    } else if (card.type == "Defense") 
    {
        cout << "│ " << left << setw(50) << ("Defense: " + to_string(card.defense)) << "│\n";
    } else if (card.type == "Versatile") 
    {
        cout << "│ " << left << setw(50) << ("Attack/Defense: " + to_string(card.attack) + "/" + to_string(card.defense)) << "│\n";
    }
    
    if (!card.timing.empty()) 
    {
        cout << "│ " << left << setw(50) << ("Timing: " + card.timing) << "│\n";
    }
        
        cout << "│ " << left << setw(50) << ("Boost: " + to_string(card.boost)) << "│\n";
        cout << "│ " << left << setw(50) << ("Quantity: x" + to_string(card.quantity)) << "│\n";
    
    if (card.isExtra) 
    {
        cout << "│ " << left << setw(50) << "★ Extra Card" << "│\n";
    }
    
        cout << "├────────────────────────────────────────────────────┤\n";
    
    string effect = card.effect;
    int maxWidth = 48;
    while (effect.length() > maxWidth) 
    {
        int space = effect.rfind(' ', maxWidth);
        if (space == string::npos) space = maxWidth;
        cout << "│ " << left << setw(50) << effect.substr(0, space) << "│\n";
        effect = effect.substr(space + 1);
    }
    if (!effect.empty()) 
    {
        cout << "│ " << left << setw(50) << effect << "│\n";
    }
    
    cout << "└────────────────────────────────────────────────────┘\n";
}

void Deck::showDeck_SH() const 
{
    cout << "\n═══════════════════════════════════════════════════════\n";
    cout << "  SHERLOCK HOLMES DECK (" << cardsSH.size() << " cards)\n";
    cout << "═══════════════════════════════════════════════════════\n\n";
    
    for (int i = 0; i < cardsSH.size(); i++) 
    {
        cout << "[" << i + 1 << "]\n";
        showCard(cardsSH[i]);
        cout << "\n";
    }

}


void Deck::showDeck_DR() const
{
    
    cout << "\n═══════════════════════════════════════════════════════\n";
    cout << "  DRACULA DECK (" << cardsDR.size() << " cards)\n";
    cout << "═══════════════════════════════════════════════════════\n\n";
    
    for (int i = 0; i < cardsDR.size(); i++) 
    {
        cout << "[" << i + 1 << "]\n";
        showCard(cardsDR[i]);
        cout << "\n";
    }
}

void Deck::cardHandSH()
{
    HandSH.clear();
    if (cardsSH.empty()) 
    {
        cout << "Deck is empty! Cannot draw cards.\n";
        return;
    }

    
    int count = 5;

    for (int i = 0; i < count; i++) 
    {
        int rn = rand() % cardsSH.size();
        HandSH.push_back(cardsSH[rn]);
        cardsSH.erase(cardsSH.begin() + rn);

    }
    
    cout << count << " cards drawn to hand successfully!\n";//باید شرط برای برسیس اینکه بیشتر از 7 تا کارت توی دست نباشه رو رو هم بنویسی 

}


void Deck::cardHandDR()
{

    HandDR.clear();
    if (cardsDR.empty()) 
    {
        cout << "Deck is empty! Cannot draw cards.\n";
        return;
    }

    
    int count = 5;

    for (int i = 0; i < count; i++) 
    {
        int rn = rand() % cardsDR.size();
        HandDR.push_back(cardsSH[rn]);
        cardsSH.erase(cardsSH.begin() + rn);
        
    }
    
    cout << count << " cards drawn to hand successfully!\n";

}

void Deck::showHandSH() const
{
    cout << "\n═══════════════════════════════════════════════════════\n";
    cout << "  DRACULA DECK (" << HandSH.size() << " Handcards)\n";
    cout << "═══════════════════════════════════════════════════════\n\n";

    for (int i = 0; i <HandSH.size() ; i++)
    {
        cout << "[" << i + 1 << "]\n";
        showCard(HandSH[i]);
        cout << "\n";
    }
}

void Deck::showHandDR() const
{

    cout << "\n═══════════════════════════════════════════════════════\n";
    cout << "  DRACULA DECK (" << HandDR.size() << " Handcards)\n";
    cout << "═══════════════════════════════════════════════════════\n\n";

    for (int i = 0; i <HandDR.size() ; i++)
    {
        cout << "[" << i + 1 << "]\n";
        showCard(HandDR[i]);
        cout << "\n";
    }
    


}


Card* Deck::selectAndRemoveFromHandSH(int index) 
{
    if (index < 0 || index >= HandSH.size()) 
    {
        return nullptr;
    }
    
    Card* selectedCard = &HandSH[index];
    HandSH.erase(HandSH.begin() + index);
    return selectedCard;
}

Card* Deck::selectAndRemoveFromHandDR(int index) 
{
    if (index < 0 || index >= HandDR.size()) 
    {
        return nullptr;
    }
    
    Card* selectedCard = &HandDR[index];
    HandDR.erase(HandDR.begin() + index);
    return selectedCard;
}


bool Deck::isHandSHEmpty() const 
{
    return HandSH.empty();
}

bool Deck::isHandDREmpty() const 
{
    return HandDR.empty(); 
}

int  Deck::getHandSHSize() const 
{
    return HandSH.size(); 
}


int  Deck::getHandDRSize() const 
{ 
    return HandDR.size();
}

vector<Card> &Deck::getCardsSH() 
{
    return cardsSH;
}

vector<Card> &Deck::getCardsDR() 
{
    return cardsDR;
}


std::vector<Card> Deck::getCardsH()
{
    return HandSH;
}


std::vector<Card> Deck::getCardDR()
{
    return HandDR;
}




Deck::~Deck() 
{
    
}