#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

class TextureManager
{
private:
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::Font> fonts;

public:
    TextureManager() = default;

    
    sf::Texture& getTexture(const std::string& key, const std::string& path);
    sf::Font& getFont(const std::string& key, const std::string& path);

    ~TextureManager() = default;
};