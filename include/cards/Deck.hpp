#pragma once
#include <vector>
#include "Card.hpp"

class Deck {
private:
    std::vector<Card> cardsSH;
    std::vector<Card> cardsDR;

public:
    Deck();
    void SherlockDeck();
    void DraculaDeck();
    void showDeck() const;
    void showCard(const Card& card) const;
    std::vector<Card> getCardsSH() const;
    std::vector<Card> getCardsDR() const;
    ~Deck();
};

