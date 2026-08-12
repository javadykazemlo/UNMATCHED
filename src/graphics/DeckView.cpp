#include "graphics/DeckView.hpp"

void DeckView::draw(sf::RenderTarget& target, sf::Vector2f position,
                    int deckSize, int discardSize) const
{
    sf::RectangleShape deck({72.f, 96.f});
    deck.setPosition(position);
    deck.setFillColor(sf::Color(24, 20, 25));
    deck.setOutlineColor(sf::Color(118, 91, 58));
    deck.setOutlineThickness(2.f);
    target.draw(deck);

    sf::RectangleShape inner({56.f, 80.f});
    inner.setPosition({position.x + 8.f, position.y + 8.f});
    inner.setFillColor(sf::Color(39, 29, 33));
    inner.setOutlineColor(sf::Color(158, 130, 86));
    inner.setOutlineThickness(1.f);
    target.draw(inner);
}
