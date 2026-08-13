#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <string>

class Bord;
class TextureManager;

class BoardView
{
private:
    std::array<sf::Vector2f, 32> positions{};
    const TextureManager* textures = nullptr;
    void buildLayout();
    void drawConnections(sf::RenderTarget& target, const Bord& board) const;
    void drawZones(sf::RenderTarget& target, const Bord& board) const;
    sf::Color zoneColor(int zone) const;
    int primaryZone(const Bord& board, int space) const;

public:
    explicit BoardView(const TextureManager* textures = nullptr);
    void draw(sf::RenderTarget& target, const Bord& board,
              int selectedSpace, const std::vector<int>& highlightedSpaces) const;
    int getSpaceAt(sf::Vector2f mouse) const;
    sf::Vector2f getPosition(int space) const;
};
