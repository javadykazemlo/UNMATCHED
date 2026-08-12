#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Card;

class CardView
{
public:
    CardView(const sf::Font& font);

    void drawHand(sf::RenderTarget& target, const std::vector<Card>& cards,
                  int selectedIndex) const;

    int getCardAt(sf::Vector2f mouse, int cardCount) const;

private:
    const sf::Font& font;

    void drawCard(sf::RenderTarget& target, const Card& card,
                  sf::FloatRect rect, bool selected) const;
};
