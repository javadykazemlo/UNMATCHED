#include "graphics/CharacterView.hpp"
#include "graphics/TextureManager.hpp"
#include "entities/Character.hpp"
#include <algorithm>

CharacterView::CharacterView(const sf::Font& font_, const TextureManager& textures_)
    : font(font_), textures(textures_) {}

sf::Color CharacterView::ownerColor(int owner) const
{
    return owner == 1 ? sf::Color(145, 35, 39) : sf::Color(38, 82, 124);
}

std::string CharacterView::shortName(const std::string& name) const
{
    if (name == "Dracula") return "D";
    if (name == "sherlock") return "S";
    if (name == "Dr_watson") return "W";
    if (name == "invisible man") return "I";
    if (name.find("Sister") != std::string::npos) return "S";
    return "?";
}

std::string CharacterView::textureId(const std::string& name) const
{
    if (name == "Dracula") return "dracula";
    if (name == "sherlock") return "sherlock";
    if (name == "Dr_watson") return "watson";
    if (name == "invisible man") return "invisible_man";
    if (name.find("Sister") != std::string::npos) return "sisters";
    return {};
}

void CharacterView::draw(sf::RenderTarget& target, Character* character,
                         sf::Vector2f position, bool selected) const
{
    if (!character || !character->checkalive()) return;

    const sf::Color accent = ownerColor(character->getowner());
    sf::CircleShape token(27.f);
    token.setOrigin({27.f,27.f});
    token.setPosition(position);
    token.setFillColor(accent);
    token.setOutlineColor(selected ? sf::Color(239, 202, 119) : sf::Color(220, 205, 171));
    token.setOutlineThickness(selected ? 4.f : 2.f);
    target.draw(token);

    const sf::Texture* texture = textures.get(textureId(character->getName()));
    if (texture)
    {
        sf::Sprite sprite(*texture);
        const sf::Vector2u size = texture->getSize();
        const float scale = std::min(42.f / static_cast<float>(size.x),
                                     42.f / static_cast<float>(size.y));
        sprite.setScale({scale, scale});
        const sf::FloatRect b = sprite.getGlobalBounds();
        sprite.setPosition({position.x - b.size.x / 2.f, position.y - b.size.y / 2.f});
        target.draw(sprite);
    }
    else
    {
        sf::Text letter(font, shortName(character->getName()), 24);
        const auto bounds = letter.getLocalBounds();
        letter.setOrigin({bounds.position.x + bounds.size.x / 2.f,
                          bounds.position.y + bounds.size.y / 2.f});
        letter.setPosition(position);
        letter.setFillColor(sf::Color(245, 232, 201));
        target.draw(letter);
    }
}
