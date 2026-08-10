#include "graphics/TextureManager.hpp"
#include <iostream>

TextureManager& TextureManager::instance()
{
    static TextureManager mgr;
    return mgr;
}

sf::Texture& TextureManager::getTexture(const std::string& key, const std::string& path)
{
    auto it = textures_.find(key);
    if (it != textures_.end())
        return it->second;

    sf::Texture tex;
    if (!tex.loadFromFile(path))
    {
        std::cerr << "[TextureManager] could not load texture '" << path
                   << "', using placeholder instead.\n";
        return buildPlaceholder(sf::Color(70, 60, 80));
    }
    tex.setSmooth(true);
    auto result = textures_.emplace(key, std::move(tex));
    return result.first->second;
}

sf::Font& TextureManager::getFont(const std::string& key, const std::string& path)
{
    auto it = fonts_.find(key);
    if (it != fonts_.end())
        return it->second;

    sf::Font font;
    if (!font.openFromFile(path))
    {
        std::cerr << "[TextureManager] could not load font '" << path << "'.\n";
    }
    auto result = fonts_.emplace(key, std::move(font));
    return result.first->second;
}

const sf::Font& TextureManager::titleFont()
{
    return getFont("cinzel", "assets/fonts/Cinzel-Bold.ttf");
}

const sf::Font& TextureManager::bodyFont()
{
    return getFont("cinzel", "assets/fonts/Cinzel-Bold.ttf");
}

sf::Texture& TextureManager::buildPlaceholder(sf::Color tint)
{
    std::uint32_t key = tint.toInteger();
    auto it = placeholders_.find(key);
    if (it != placeholders_.end())
        return it->second;

    sf::Image img({64, 64}, tint);
    sf::Texture tex;
    (void)tex.loadFromImage(img);
    auto result = placeholders_.emplace(key, std::move(tex));
    return result.first->second;
}

sf::Texture& TextureManager::getPlaceholder(sf::Color tint)
{
    return buildPlaceholder(tint);
}