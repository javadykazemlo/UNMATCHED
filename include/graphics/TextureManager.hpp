#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

// ─────────────────────────────────────────────────────────────────────────
//  TextureManager
//  Single place responsible for loading and caching every sf::Texture and
//  sf::Font used by the graphics layer. Nothing else opens a font/image
//  file directly. Falls back to a generated placeholder when an asset is
//  missing so the game never crashes because of a missing .png.
// ─────────────────────────────────────────────────────────────────────────
class TextureManager
{
public:
    static TextureManager& instance();

    // Generic cache access. Loads from disk on first request, returns the
    // cached instance afterwards. Returns a valid (placeholder) reference
    // even if the file could not be found.
    sf::Texture& getTexture(const std::string& key, const std::string& path);
    sf::Font&    getFont(const std::string& key, const std::string& path);

    // Convenience helpers used across the graphics module -------------------
    const sf::Font& titleFont();                    // Cinzel-Bold.ttf
    const sf::Font& bodyFont();                      // same file, body sizes
    sf::Texture&     getPlaceholder(sf::Color tint);  // flat color texture

private:
    TextureManager() = default;
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    std::unordered_map<std::string, sf::Texture> textures_;
    std::unordered_map<std::string, sf::Font> fonts_;
    std::unordered_map<std::uint32_t, sf::Texture> placeholders_;

    sf::Texture& buildPlaceholder(sf::Color tint);
};