#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <random>
#include <stdexcept>

#include "cards/Deck.hpp"
#include "GameTUI.hpp"

using namespace std;

Deck::Deck(int hero)
{
    buildDeck(hero);
    shuffle();
    draw(5);
}

void Deck::buildDeck(int hero)
{
    deck.clear();

    if(hero == 1)
    {
        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Feeding Frenzy",           //name
                owner::Hero,                //Owner
                CardType::Attack,           //Type
                2,                          //Attack
                timing::DuringCombat,       //timing
                3,                          //boost
                "This attack gains +1 for each Sister in the same zone as the opposing fighter."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "MistForm",
                owner::Hero,
                CardType::Scheme,
                0,
                timing::None,
                2,
                "Place Dracula in any space and gain 1 action."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Ambush",
                owner::Any,
                CardType::Attack,
                2,
                timing::DuringCombat,
                3,
                "Opponent discards a random card. Add its boost value to this attack."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Baptism of Blood",
                owner::Hero,
                CardType::Scheme,
                0,
                timing::None,
                2,
                "Recover 2 health. If a Sister is defeated, return her to any Dracula space."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "BeastForm",
                owner::Hero,
                CardType::Attack,
                6,
                timing::DuringCombat,
                4,
                "Discard any number of cards. Gain +1 attack for each discarded card."
            );
        }

        for (int i = 0; i < 3; i++) 
        {
            deck.emplace_back(
                "Dash",
                owner::Any,
                CardType::Versatile,
                3,
                timing::AfterCombat,
                1,
                "Move your fighter up to 3 spaces."
            );
        }

        for (int i = 0; i < 3; i++) 
        {
            deck.emplace_back(
                "Exploit",
                owner::Any,
                CardType::Versatile,
                4,
                timing::AfterCombat,
                1,
                "Draw 1 card."
            );
        }
            
        for (int i = 0; i < 3; i++) 
        {
            deck.emplace_back(
                "Look Into My Eyes",
                owner::Hero,
                CardType::Defense,
                1,
                timing::DuringCombat,
                2,
                "Add the boost value of your opponent's attack card to this defense."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Prey Upon",
                owner::Hero,
                CardType::Scheme,
                0,
                timing::None,
                4,
                "Deal 1 damage to all adjacent opposing fighters. Dracula heals 1 for each damage dealt."
            );
        }

        for (int i = 0; i < 3; i++) 
        {
            deck.emplace_back(
                "Ravening Seduction",
                owner::SideKick,
                CardType::Scheme,
                0,
                timing::None,
                2,
                "Move any fighter up to 2 spaces. Then deal 1 damage for each adjacent Sister."
            );
        }
        
        for (int i = 0; i < 3; i++) 
        {
            deck.emplace_back(
                "Thirst for Sustenance",
                owner::SideKick,
                CardType::Attack,
                3,
                timing::AfterCombat,
                3,
                "If you won the combat, place Dracula adjacent to the opposing fighter."
            );
        }
        
        for (int i = 0; i < 3; i++) 
        {
            deck.emplace_back(
                "Feint",
                owner::Any,
                CardType::Versatile,
                2,
                timing::BeforeCombat,
                2,
                "Cancel all effects on your opponent's card."
            );
        }


    }
    else if(hero == 2)
    {

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Administer Aid",   
                owner::SideKick,                 
                CardType::Scheme,                  
                0,                  
                timing::None,                  
                2,                  
                "Place Watson adjacent to Holmes. Heal Holmes by 1 and draw 1 card."
            );
        }

        for (int i = 0; i < 3; i++) 
        {
            deck.emplace_back(
                "Confirm Suspicion",
                owner::Hero,
                CardType::Scheme,
                0,
                timing::None,
                1,
                "Name an attack or defense value. Your opponent must reveal and discard a matching card. \nThe opposing hero suffers damage equal to that card's boost value. Otherwise they reveal their hand."
            );
        }

        for (int i = 0; i < 3; i++) 
        {
            deck.emplace_back(
                "Counterpunch",
                owner::Hero,
                CardType::Versatile,
                3,
                timing::AfterCombat,
                1,
                "If Holmes is adjacent to the opposing fighter, deal 2 damage."
            );
        }

        for (int i = 0; i < 3; i++) 
        {
            deck.emplace_back(
                "Deduce Strategy",
                owner::Hero,
                CardType::Versatile,
                3,
                timing::DuringCombat,
                1,
                "You may change your opponent's printed combat value to its boost value."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Education Never Ends",
                owner::Any,
                CardType::Versatile,
                3, 
                timing::AfterCombat,
                1,
                "If you won, your opponent draws 1 card. Otherwise draw 2 cards."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Elementary",
                owner::Hero,
                CardType::Defense,
                3,
                timing::DuringCombat,
                3,
                "Guess the attack value. If correct, ignore the opponent's attack and cancel all effects."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Eliminate the Impossible",
                owner::Hero,
                CardType::Scheme,
                0,
                timing::None,
                2,
                "Look at your opponent's hand and discard one card."
            );
        }

        for (int i = 0; i < 3; i++) 
        {
            deck.emplace_back(
                "Feint",
                owner::Any,
                CardType::Versatile,
                2,
                timing::BeforeCombat,
                1,
                "Cancel all effects on your opponent's card."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Fixed Point in a Changing Age",
                owner::SideKick,
                CardType::Versatile,
                3,
                timing::AfterCombat,
                1,
                "If Watson is adjacent to Holmes, both recover 1 health."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Master of Disguise",
                owner::Hero,
                CardType::Scheme,
                0,
                timing::None,
                2,
                "Swap Holmes with an opposing fighter and deal 1 damage."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "The Game is Afoot",
                owner::Hero,
                CardType::Attack,
                5,
                timing::AfterCombat,
                2,
                "Move Holmes up to 3 spaces."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Service Revolver",
                owner::SideKick,
                CardType::Attack,
                5,
                timing::None,
                3,
                "No additional effect."
            );
        }

        for (int i = 0; i < 2; i++) 
        {
            deck.emplace_back(
                "Study Methods",
                owner::Any,
                CardType::Versatile,
                3,
                timing::AfterCombat,
                3,
                "If you won the combat, you may look at your opponent's hand."
            );
        }
        
    }
    else if(hero == 3)
    {

        for (int i = 0; i < 2; i++)
        {
            deck.emplace_back(
                "Coded Notes",
                owner::Hero,
                CardType::Defense,
                3,
                timing::AfterCombat,
                2,
                "After the fight, draw a few cards and quietly slide a couple of them back on top of your deck in whichever order suits you."
            );
        }

        for (int i = 0; i < 2; i++)
        {
            deck.emplace_back(
                "Confound",
                owner::Hero,
                CardType::Versatile,
                3,
                timing::AfterCombat,
                2,
                "Your opponent may discard a card to stop you;\n otherwise you scatter every fog token to new spaces."
            );
        }

        for (int i = 0; i < 3; i++)
        {
            deck.emplace_back(
                "Covert Preparation",
                owner::Hero,
                CardType::Versatile,
                2,
                timing::AfterCombat,
                1,
                "Draw a card, then reposition a fog token a short distance across the board."
            );
        }

        for (int i = 0; i < 2; i++)
        {
            deck.emplace_back(
                "Dreaming of Revenge",
                owner::Hero,
                CardType::Versatile,
                3,
                timing::AfterCombat,
                1,
                "If you are standing in the fog, every enemy fighter sharing a fog space with it feels the sting too."
            );
        }

        for (int i = 0; i < 2; i++)
        {
            deck.emplace_back(
                "Emerge from Mist",
                owner::Hero,
                CardType::Attack,
                3,
                timing::DuringCombat,
                2,
                "Stepping out of the fog to strike hits far harder than anyone expects."
            );
        }

        for (int i = 0; i < 2; i++)
        {
            deck.emplace_back(
                "Impossible to See",
                owner::Hero,
                CardType::Versatile,
                2,
                timing::BeforeCombat,
                2,
                "Your opponent's card is reduced to nothing before it ever has a chance to matter."
            );
        }

        for (int i = 0; i < 2; i++)
        {
            deck.emplace_back(
                "Into Thin Air",
                owner::Hero,
                CardType::Defense,
                4,
                timing::AfterCombat,
                1,
                "Slip a single space away while your opponent is forced to relocate one of your fog tokens."
            );
        }

        for (int i = 0; i < 3; i++)
        {
            deck.emplace_back(
                "Lurking",
                owner::Hero,
                CardType::Defense,
                2,
                timing::AfterCombat,
                2,
                "Draw a card, then either slip onto a fog space or nudge a fog token further away."
            );
        }

        for (int i = 0; i < 2; i++)
        {
            deck.emplace_back(
                "Reign of Terror",
                owner::Hero,
                CardType::Scheme,
                0,
                timing::None,
                1,
                "From the safety of the fog, strike fear and damage into a chosen enemy fighter."
            );
        }

        for (int i = 0; i < 2; i++)
        {
            deck.emplace_back(
                "Rolling Fog",
                owner::Hero,
                CardType::Scheme,
                0,
                timing::None,
                1,
                "Shift a fog token to another space and keep the pressure on with an extra action."
            );
        }

        for (int i = 0; i < 3; i++)
        {
            deck.emplace_back(
                "Slip Away",
                owner::Hero,
                CardType::Attack,
                3,
                timing::AfterCombat,
                2,
                "Move a fog token to an empty space, then vanish into it yourself."
            );
        }

        for (int i = 0; i < 2; i++)
        {
            deck.emplace_back(
                "Step Lightly",
                owner::Hero,
                CardType::Scheme,
                0,
                timing::None,
                1,
                "A quick strike on a nearby fighter, made far more dangerous when thrown from the fog."
            );
        }

        for (int i = 0; i < 3; i++)
        {
            deck.emplace_back(
                "Vanish",
                owner::Hero,
                CardType::Scheme,
                0,
                timing::None,
                3,
                "Recover some health and disappear from the board entirely, until you choose to reappear."
            );
        }

    }

}

void Deck::shuffle()
{

    random_device rd;
    mt19937 gen(rd());
    std::shuffle(deck.begin(), deck.end(), gen);
    
}

void Deck::draw(int count)
{
    for(int i = 0 ; i < count ; i++)
    {
        if(deck.empty())
        {
            throw runtime_error("Deck is empty");
        }
        else
        {
            hand.push_back(deck.back());
            deck.pop_back();
        }
    }
}

Card Deck::playCard(int index, Card& selected)
{
    if(index < 0 || index >= hand.size())
    return selected;

    selected = hand[index];
    hand.erase(hand.begin() + index);

    DiscardPile.push_back(selected);

    return selected;
}


void Deck::discardCard(const Card& card)
{
    DiscardPile.push_back(card);
}


Card Deck::removeFromHand(int index)
{
    Card selected;
    if(index < 0 || index >= (int)hand.size())
        return selected;

    selected = hand[index];
    hand.erase(hand.begin() + index);

    return selected;
}

void Deck::putOnTop(const Card& card)
{
    deck.push_back(card);
}


void Deck::showCard(const Card& card) const 
{
    cout << "┌────────────────────────────────────────────────────┐\n";
    cout << "│ " << left << setw(50) << card.getName() << " │\n";
    cout << "├────────────────────────────────────────────────────┤\n";
    cout << "│ " << left << setw(50) << (card.getOwnerString() + " · " + card.getTypeString()) << "  │\n";
    cout << "├────────────────────────────────────────────────────┤\n";

    if (card.isAttack()) 
    {
        cout << "│ " << left << setw(50) << ("Attack: " + to_string(card.getAttack())) << "│\n";
    } else if (card.isDefense()) 
    {
        cout << "│ " << left << setw(50) << ("Defense: " + to_string(card.getAttack())) << "│\n";
    } else if (card.isVersatile()) 
    {
        cout << "│ " << left << setw(50) << ("Attack/Defense: " + to_string(card.getAttack()) + "/" + to_string(card.getAttack())) << "│\n";
    }

    cout << "│ " << left << setw(50) << ("Boost: " + to_string(card.getBoost())) << " │\n";

    cout << "├────────────────────────────────────────────────────┤\n";

    string effect = card.geteffect();
    int maxWidth = 48;
    while (effect.length() > maxWidth) 
    {
        int space = effect.rfind(' ', maxWidth);
        if (space == string::npos) space = maxWidth;
        cout << "│ " << left << setw(50) << effect.substr(0, space) << " │\n";
        effect = effect.substr(space + 1);
    }
    if (!effect.empty()) 
    {
        cout << "│ " << left << setw(50) << effect << " │\n";
    }

    cout << "└────────────────────────────────────────────────────┘\n";
    
}

void Deck::showDeck(string name) const 
{
    int width = 100;

    cout << endl;
    GameTUI g;
    for (auto& line : g.buildHandPanel(getdeck() , name , g.GREEN , width))
        cout << line << "\n";

    cout << "\n";

}

void Deck::showHand(string name) const
{
    int width = 100;

    cout << endl;
    GameTUI g;
    for (auto& line : g.buildHandPanel(gethand() , name , g.GREEN , width))
        cout << line << "\n";

    cout << "\n";
}



vector<int> Deck::getAttackCardIndices() const
{
    vector<int> indices;
    for(int i = 0; i < hand.size(); i++)
    {
        if(hand[i].isAttack() || hand[i].isVersatile())
        {
            indices.push_back(i + 1);
        }
    }

    return indices;
}



vector<int> Deck::getDefenseCardIndices() const
{
    vector<int> indices;
    for(int i = 0; i < hand.size(); i++)
    {
        if(hand[i].isDefense() || hand[i].isVersatile())
        {
            indices.push_back(i + 1);
        }
    }

    return indices;
}


vector<int> Deck::getSchemeCardIndices() const
{
    vector<int> indices;
    for(int i = 0; i < hand.size(); i++)
    {
        if(hand[i].isScheme())
        {
            indices.push_back(i + 1);
        }
    }

    return indices;
}


bool Deck::isDeckEmpty() const
{
    return deck.empty();
}

bool Deck::isHandEmpty() const 
{
    return hand.empty();
}


int Deck::getdeckSize() const
{
    return deck.size(); 
}

int Deck::gethandSize() const
{
    return hand.size(); 
}
    
int Deck::getdiscardSize() const
{
    return DiscardPile.size(); 
}

Card Deck::getHandcard(int index)
{
    Card selected = hand[index];
    return selected;
}

const vector<Card>& Deck::getdeck() const
{
    return deck;
}

const vector<Card>& Deck::gethand() const
{
    return hand;
}

const vector<Card>& Deck::getDiscardPile() const
{
    return DiscardPile;
}



void Deck::loadState(const vector<Card>& deckCards, const vector<Card>& handCards, const vector<Card>& discardCards)
{
    deck = deckCards;
    hand = handCards;
    DiscardPile = discardCards;
}