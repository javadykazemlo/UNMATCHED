#include "graphics/CharacterView.hpp"
#include "entities/Character.hpp"

sf::Color CharacterView::ownerColor(int owner) const
{
    return owner == 1 ? sf::Color(142, 33, 37) : sf::Color(36, 83, 123);
}

std::string CharacterView::shortName(const std::string& name) const
{
    if (name == "Dracula") return "D";
    if (name == "sherlock") return "SH";
    if (name == "Dr_watson") return "W";
    if (name == "invisible man") return "IM";
    if (name.find("Sister") != std::string::npos) return "S";
    return "?";
}

void CharacterView::draw(sf::RenderTarget& target, Character* character,
                         sf::Vector2f position, bool selected) const
{
    if (!character || !character->checkalive())
        return;

    sf::CircleShape token(26.f);
    token.setOrigin({26.f,26.f});
    token.setPosition(position);
    token.setFillColor(ownerColor(character->getowner()));
    token.setOutlineColor(selected ? sf::Color(231, 193, 111)
                                   : sf::Color(205, 190, 160));
    token.setOutlineThickness(selected ? 4.f : 2.f);
    target.draw(token);

    // A second inner ring makes the token readable without external assets.
    sf::CircleShape inner(20.f);
    inner.setOrigin({20.f,20.f});
    inner.setPosition(position);
    inner.setFillColor(sf::Color(15, 16, 21, 90));
    inner.setOutlineColor(sf::Color(225, 215, 190, 90));
    inner.setOutlineThickness(1.f);
    target.draw(inner);
}
