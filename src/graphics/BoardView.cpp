#include "graphics/BoardView.hpp"
#include "core/Bord.hpp"
#include "entities/Character.hpp"
#include <cmath>

BoardView::BoardView()
{
    buildLayout();
}

void BoardView::buildLayout()
{
    // Fixed visual coordinates for the 32 logical spaces.
    // These are presentation coordinates only; adjacency remains in Bord.
    const float cx = 825.f;
    const float cy = 365.f;

    positions = {
        sf::Vector2f{cx,cy-225}, {cx-105,cy-205}, {cx+105,cy-205},
        {cx-185,cy-145}, {cx-75,cy-150}, {cx+75,cy-150}, {cx+185,cy-145},
        {cx-235,cy-55}, {cx-125,cy-75}, {cx,cy-85}, {cx+125,cy-75}, {cx+235,cy-55},
        {cx-250,cy+35}, {cx-155,cy+25}, {cx-65,cy+25}, {cx+65,cy+25},
        {cx+155,cy+25}, {cx+250,cy+35},
        {cx-235,cy+125}, {cx-125,cy+105}, {cx,cy+115}, {cx+125,cy+105}, {cx+235,cy+125},
        {cx-185,cy+195}, {cx-75,cy+175}, {cx+75,cy+175}, {cx+185,cy+195},
        {cx-105,cy+245}, {cx,cy+225}, {cx+105,cy+245},
        {cx-45,cy-250}, {cx+45,cy-250}
    };
}

void BoardView::drawConnections(sf::RenderTarget& target, const Bord& board) const
{
    for (int i = 0; i < 32; ++i)
    {
        for (int j : board.getposAdjacent(i))
        {
            if (j <= i || j < 0 || j >= 32)
                continue;

            // SFML 3.x no longer provides the old Vertex(position, color)
            // constructor. Set the fields explicitly.
            sf::Vertex line[2];
            line[0].position = positions[i];
            line[0].color = sf::Color(105, 88, 62, 125);
            line[1].position = positions[j];
            line[1].color = sf::Color(105, 88, 62, 125);

            target.draw(line, 2, sf::PrimitiveType::Lines);
        }
    }
}

void BoardView::draw(sf::RenderTarget& target, const Bord& board,
                     int selectedSpace, const std::vector<int>& highlightedSpaces) const
{
    sf::RectangleShape boardPanel({610.f, 565.f});
    boardPanel.setPosition({520.f, 95.f});
    boardPanel.setFillColor(sf::Color(7, 9, 13, 245));
    boardPanel.setOutlineColor(sf::Color(105, 83, 52));
    boardPanel.setOutlineThickness(2.f);
    target.draw(boardPanel);

    sf::RectangleShape inner({592.f, 547.f});
    inner.setPosition({529.f,104.f});
    inner.setFillColor(sf::Color(14, 22, 25, 255));
    inner.setOutlineColor(sf::Color(42, 66, 64));
    inner.setOutlineThickness(1.f);
    target.draw(inner);

    // Zone-like atmospheric areas.
    sf::CircleShape lake(120.f);
    lake.setOrigin({120.f,120.f});
    lake.setPosition({825.f,380.f});
    lake.setFillColor(sf::Color(9, 34, 43, 180));
    lake.setOutlineColor(sf::Color(44, 76, 78, 160));
    lake.setOutlineThickness(2.f);
    target.draw(lake);

    drawConnections(target, board);

    for (int i = 0; i < 32; ++i)
    {
        bool highlighted = false;
        for (int h : highlightedSpaces)
            if (h == i) { highlighted = true; break; }

        sf::CircleShape space(18.f);
        space.setOrigin({18.f,18.f});
        space.setPosition(positions[i]);

        if (i == selectedSpace)
            space.setFillColor(sf::Color(196, 148, 61, 235));
        else if (highlighted)
            space.setFillColor(sf::Color(83, 119, 94, 235));
        else
            space.setFillColor(sf::Color(38, 45, 47, 235));

        space.setOutlineColor(sf::Color(190, 164, 111, 180));
        space.setOutlineThickness(1.5f);
        target.draw(space);

        if (board.getCharacter(i))
        {
            sf::CircleShape occupied(11.f);
            occupied.setOrigin({11.f,11.f});
            occupied.setPosition(positions[i]);
            occupied.setFillColor(board.getCharacter(i)->getowner() == 1
                                  ? sf::Color(125, 25, 28)
                                  : sf::Color(31, 73, 110));
            occupied.setOutlineColor(sf::Color(230, 215, 176));
            occupied.setOutlineThickness(1.f);
            target.draw(occupied);
        }
    }
}

int BoardView::getSpaceAt(sf::Vector2f mouse) const
{
    float best = 9999.f;
    int index = -1;

    for (int i = 0; i < 32; ++i)
    {
        float dx = mouse.x - positions[i].x;
        float dy = mouse.y - positions[i].y;
        float d = std::sqrt(dx*dx + dy*dy);

        if (d < 25.f && d < best)
        {
            best = d;
            index = i;
        }
    }

    return index;
}

sf::Vector2f BoardView::getPosition(int space) const
{
    if (space < 0 || space >= 32)
        return {-100.f, -100.f};
    return positions[space];
}
