#include <iostream>
#include <string>
#include "Deck.hpp"

using namespace std;
#include "Deck.hpp"

Deck::Deck() 
{

}

void Deck::SherlockDeck() 
{

    cardsSH.clear();

    // Administer Aid x2
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

    // Confirm Suspicion x3
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

    // Counter Punch x3
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

    // Deduce Strategy x3
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

    // Education Never Ends x2
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

    // Elementary x2
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

    // Eliminate the Impossible x2
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

    // Feint x3
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

    // Fixed Point in a Changing Age x2
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

    // Master of Disguise x2
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

    // The Game Is Afoot x2
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

    // Service Revolver x2
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

    // Study Methods x2
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

    // Feeding Frenzy x2
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

    // Mistform x2
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

    // Ambush x2
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

    // Baptism of Blood x2
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

    // Beastform x2
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

    // Dash x3
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

    // Exploit x3
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

    // Look Into My Eyes x3
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

    // Prey Upon x2
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

    // Ravening Seduction x3
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

    // Thirst for Sustenance x3
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

    // Feint x3
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

void Deck::showDeck() const
{

}

std::vector<Card> Deck::getCardsSH() const 
{
    return cardsSH;
}

std::vector<Card> Deck::getCardsDR() const 
{
    return cardsDR;
}

Deck::~Deck() 
{

}