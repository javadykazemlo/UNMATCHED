#pragma once
#include <SFML/Graphics.hpp>
#include "cards/Card.hpp"

// ─────────────────────────────────────────────────────────────────────────
//  CardView
//  Draws exactly one Card as a gothic playing-card: colored header strip
//  (by CardType), name, attack/boost value, timing tag and wrapped effect
//  text. Can be shown face down (card back) for the deck/opponent's hand.
//  Pure presentation: it only reads a Card by const reference, it never
//  mutates the game state.
// ─────────────────────────────────────────────────────────────────────────
class CardView
{
public:
    CardView(sf::Vector2f position = {0.f, 0.f}, sf::Vector2f size = {120.f, 168.f});

    void setCard(const Card& card);
    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void setFaceDown(bool faceDown);
    void setSelected(bool selected);
    void setHighlighted(bool highlighted); // e.g. "playable" glow
    void setIndex(int handIndex); // 1-based index shown in the corner, -1 = hidden

    sf::Vector2f getPosition() const { return position_; }
    sf::Vector2f getSize() const { return size_; }
    sf::FloatRect getBounds() const;
    bool contains(sf::Vector2f point) const;

    void updateHover(sf::Vector2f mousePos);
    void draw(sf::RenderTarget& target) const;

    static sf::Color colorForType(CardType type);

private:
    sf::Vector2f position_;
    sf::Vector2f size_;
    Card card_;
    bool hasCard_ = false;
    bool faceDown_ = false;
    bool selected_ = false;
    bool highlighted_ = false;
    bool hovered_ = false;
    int index_ = -1;
};