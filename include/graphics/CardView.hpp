#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Card;
class TextureManager;

class CardView
{
private:
    const sf::Font& font;
    const TextureManager* textures = nullptr;
    std::string textureId(const Card& card) const;
    void drawWrappedText(sf::RenderTarget& target, const std::string& text,
                         sf::Vector2f position, unsigned size,
                         float maxWidth, sf::Color color, int maxLines = 4) const;
public:
    explicit CardView(const sf::Font& font, const TextureManager* textures = nullptr);
    void drawCard(sf::RenderTarget& target, const Card& card,
                  sf::FloatRect rect, bool selected) const;
    void drawHand(sf::RenderTarget& target, const std::vector<Card>& cards,
                  int selectedIndex) const;
    int getCardAt(sf::Vector2f mouse, int cardCount) const;
};
