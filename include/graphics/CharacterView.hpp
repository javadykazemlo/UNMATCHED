#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Character;

class CharacterView
{
public:
    CharacterView() = default;

    void draw(sf::RenderTarget& target, Character* character,
              sf::Vector2f position, bool selected) const;

private:
    sf::Color ownerColor(int owner) const;
    std::string shortName(const std::string& name) const;
};
