#include "graphics/DeckView.hpp"
#include "graphics/TextureManager.hpp"

DeckView::DeckView(const sf::Font& font_, const TextureManager* textures_) : font(font_), textures(textures_) {}

void DeckView::draw(sf::RenderTarget& target, sf::Vector2f position,
                    int deckSize, int handSize, int discardSize) const
{
    if (textures)
    {
        if (const sf::Texture* texture = textures->get("card_back"))
        {
            sf::Sprite sprite(*texture);
            const sf::Vector2u size = texture->getSize();
            if (size.x > 0 && size.y > 0)
            {
                sprite.setScale({66.f / static_cast<float>(size.x),
                                 88.f / static_cast<float>(size.y)});
                sprite.setPosition(position);
                target.draw(sprite);
            }
        }
        else
        {
            sf::RectangleShape deck({66.f, 88.f});
            deck.setPosition(position);
            deck.setFillColor(sf::Color(24, 20, 26));
            deck.setOutlineColor(sf::Color(145, 111, 67));
            deck.setOutlineThickness(2.f);
            target.draw(deck);
        }
    }
    else
    {
        sf::RectangleShape deck({66.f, 88.f});
        deck.setPosition(position);
        deck.setFillColor(sf::Color(24, 20, 26));
        deck.setOutlineColor(sf::Color(145, 111, 67));
        deck.setOutlineThickness(2.f);
        target.draw(deck);
    }

    if (!textures || !textures->get("card_back"))
    {
        sf::RectangleShape inner({52.f, 74.f});
        inner.setPosition({position.x + 7.f, position.y + 7.f});
        inner.setFillColor(sf::Color(39, 28, 34));
        inner.setOutlineColor(sf::Color(177, 145, 91));
        inner.setOutlineThickness(1.f);
        target.draw(inner);

        sf::Text mark(font, "DECK", 8);
        mark.setPosition({position.x + 17.f, position.y + 32.f});
        mark.setFillColor(sf::Color(220, 201, 164));
        target.draw(mark);
    }

    sf::Text counts(font,
        std::to_string(deckSize) + " / " + std::to_string(handSize) + " / " + std::to_string(discardSize),
        8);
    counts.setPosition({position.x - 4.f, position.y + 94.f});
    counts.setFillColor(sf::Color(180, 171, 156));
    target.draw(counts);
}
