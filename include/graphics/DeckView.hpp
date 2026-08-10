#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "cards/Card.hpp"
#include "graphics/CardView.hpp"

// ─────────────────────────────────────────────────────────────────────────
//  DeckView
//  Lays out the current player's hand as a fanned row of CardView objects,
//  plus small counters for the draw pile and the discard pile. Reports
//  which 1-based hand index the user clicked so GameWindow can forward it
//  to the game engine exactly the way the console version expects it.
// ─────────────────────────────────────────────────────────────────────────
class DeckView
{
public:
    DeckView(sf::Vector2f position, sf::Vector2f size);

    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);

    // Rebuilds the CardView row from the player's current hand.
    void setHand(const std::vector<Card>& hand);
    void setDeckCount(int count);
    void setDiscardCount(int count);
    void setExpanded(bool expanded); // "tap to view hand" toggle
    bool isExpanded() const { return expanded_; }

    // Highlights hand indices (1-based) that are legal to pick right now.
    void setPlayableIndices(const std::vector<int>& indices);
    void clearPlayableIndices();

    void updateHover(sf::Vector2f mousePos);
    // returns 1-based hand index clicked, or -1 if nothing relevant was hit.
    // also toggles the expand/collapse strip when that area is clicked.
    int click(sf::Vector2f mousePos);

    void draw(sf::RenderTarget& target) const;

private:
    sf::Vector2f position_, size_;
    std::vector<CardView> cardViews_;
    std::vector<int> playable_;
    int deckCount_ = 0;
    int discardCount_ = 0;
    bool expanded_ = true;

    void layout();
};