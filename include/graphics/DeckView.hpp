#pragma once
#include <SFML/Graphics.hpp>

class TextureManager;

class DeckView
{
private:
    const sf::Font& font;
    const TextureManager* textures = nullptr;
public:
    explicit DeckView(const sf::Font& font, const TextureManager* textures = nullptr);
    void draw(sf::RenderTarget& target, sf::Vector2f position,
              int deckSize, int handSize, int discardSize) const;
};
