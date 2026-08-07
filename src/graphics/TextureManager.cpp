#include "graphics/TextureManager.hpp"
#include <stdexcept>

sf::Texture& TextureManager::getTexture(const std::string& key, const std::string& path)
{
    auto it = textures.find(key);
    if (it != textures.end())
        return it->second;

    sf::Texture tex;
    if (!tex.loadFromFile(path))
        throw std::runtime_error("Failed to load texture: " + path);

    auto [inserted, ok] = textures.emplace(key, std::move(tex));
    return inserted->second;
}

sf::Font& TextureManager::getFont(const std::string& key, const std::string& path)
{
    auto it = fonts.find(key);
    if (it != fonts.end())
        return it->second;

    sf::Font font;
    if (!font.openFromFile(path))
        throw std::runtime_error("Failed to load font: " + path);

    auto [inserted, ok] = fonts.emplace(key, std::move(font));
    return inserted->second;
}