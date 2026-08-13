#include "graphics/TextureManager.hpp"

bool TextureManager::load(const std::string& id, const std::string& path)
{
    sf::Texture texture;
    if (!texture.loadFromFile(path)) return false;
    textures.insert_or_assign(id, std::move(texture));
    return true;
}

const sf::Texture* TextureManager::get(const std::string& id) const
{
    const auto it = textures.find(id);
    return it == textures.end() ? nullptr : &it->second;
}

void TextureManager::clear()
{
    textures.clear();
}
