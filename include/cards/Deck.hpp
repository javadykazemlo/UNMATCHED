#pragma once
#include <vector>
#include "Card.hpp"

class Deck {
private:

    std::vector<Card> &cardsSH;
    std::vector<Card> &cardsDR;

public:
    Deck();
    void SherlockDeck();
    void DraculaDeck();
    void showDeck_SH() const;
    void showDeck_DR() const;
    void showCard(const Card& card) const;
    std::vector<Card> &getCardsSH();
    std::vector<Card> &getCardsDR();
    ~Deck();
};
