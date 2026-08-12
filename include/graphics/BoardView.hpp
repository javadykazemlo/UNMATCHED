#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>

class Bord;
class Character;

class BoardView
{
public:
    BoardView();

    void draw(sf::RenderTarget& target, const Bord& board,
              int selectedSpace, const std::vector<int>& highlightedSpaces) const;

    int getSpaceAt(sf::Vector2f mouse) const;
    sf::Vector2f getPosition(int space) const;
    const std::array<sf::Vector2f, 32>& getPositions() const { return positions; }

private:
    std::array<sf::Vector2f, 32> positions;
    std::array<std::vector<int>, 32> links;

    void buildLayout();
    void drawConnections(sf::RenderTarget& target, const Bord& board) const;
};
