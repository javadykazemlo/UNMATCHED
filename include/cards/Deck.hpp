#pragma once
#include <string>
#include <vector>
#include "cards/Card.hpp"

class Deck 
{
private:
    std::vector<Card> deck;
    std::vector<Card> hand;
    std::vector<Card> DiscardPile;
public:
    Deck(int hero);

    void buildDeck(int hero);

    void shuffle();

    void draw(int count = 1);
    Card playCard(int index, Card& selected);
    void discardCard(const Card& card);
    
    void showCard(const Card& card) const;
    void showDeck(std::string name) const;
    void showHand(std::string name) const;

    std::vector<int> getAttackCardIndices() const;
    std::vector<int> getDefenseCardIndices() const;
    std::vector<int> getSchemeCardIndices() const;

    bool isDeckEmpty() const;
    bool isHandEmpty() const;

    int getdeckSize() const;
    int gethandSize() const;
    int getdiscardSize() const;

    Card getHandcard(int index);
    
    const std::vector<Card>& getdeck() const;
    const std::vector<Card>& gethand() const;
    ~Deck() = default;
};