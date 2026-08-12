#pragma once
#include <SFML/Graphics.hpp>

class DeckView
{
public:
    void draw(sf::RenderTarget& target, sf::Vector2f position,
              int deckSize, int discardSize) const;
};
