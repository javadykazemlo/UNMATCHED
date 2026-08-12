#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Character;
class TextureManager;

class CharacterView
{
private:
    const sf::Font& font;
    const TextureManager& textures;
    sf::Color ownerColor(int owner) const;
    std::string shortName(const std::string& name) const;
    std::string textureId(const std::string& name) const;

public:
    CharacterView(const sf::Font& font, const TextureManager& textures);
    void draw(sf::RenderTarget& target, Character* character,
              sf::Vector2f position, bool selected) const;
};
