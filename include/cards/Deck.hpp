#pragma once
#include <vector>
#include "Card.hpp"

class Deck {
private:

    std::vector<Card> &cardsSH;
    std::vector<Card> &cardsDR;
    std::vector<Card> HandSH;
    std::vector<Card> HandDR;

public:
    Deck(std::vector<Card>& sh, std::vector<Card>& dr);
    void SherlockDeck();
    void DraculaDeck();
    void showDeck_SH() const;
    void showDeck_DR() const;
    void showCard(const Card& card) const;
    void cardHandSH();
    void cardHandDR();
    std::vector<Card> &getCardsSH();
    std::vector<Card> &getCardsDR();
    std::vector<Card> getCardsH();
    std::vector<Card> getCardDR();
    ~Deck();
};
