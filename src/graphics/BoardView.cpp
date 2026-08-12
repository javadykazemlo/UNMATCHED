#include "graphics/BoardView.hpp"
#include "core/Bord.hpp"
#include "entities/Character.hpp"
#include "graphics/TextureManager.hpp"
#include <cmath>
#include <string>

namespace
{
    const sf::Color ZONE_COLORS[] = {
        sf::Color(60, 92, 64, 110),   // 1 green
        sf::Color(120, 45, 48, 105),  // 2 red
        sf::Color(48, 75, 112, 110),  // 3 blue
        sf::Color(142, 112, 48, 105), // 4 yellow/gold
        sf::Color(88, 62, 112, 110),  // 5 purple
        sf::Color(43, 100, 96, 105),  // 6 teal
        sf::Color(130, 78, 38, 105)   // 7 orange
    };
}

BoardView::BoardView(const TextureManager* textures_)
    : textures(textures_)
{
    buildLayout();
}

void BoardView::buildLayout()
{
    // Presentation-only coordinates for the exact 32 logical spaces.
    positions = {
        sf::Vector2f{800.f,135.f},
        {700.f,150.f}, {900.f,150.f},
        {620.f,205.f}, {735.f,210.f}, {865.f,210.f}, {980.f,205.f},
        {555.f,285.f}, {670.f,270.f}, {800.f,260.f}, {930.f,270.f}, {1045.f,285.f},
        {545.f,375.f}, {660.f,360.f}, {735.f,365.f}, {865.f,365.f}, {940.f,360.f}, {1055.f,375.f},
        {555.f,470.f}, {670.f,455.f}, {800.f,465.f}, {930.f,455.f}, {1045.f,470.f},
        {620.f,535.f}, {735.f,520.f}, {865.f,520.f}, {980.f,535.f},
        {700.f,575.f}, {800.f,555.f}, {900.f,575.f},
        {750.f,115.f}, {850.f,115.f}
    };
}

sf::Color BoardView::zoneColor(int zone) const
{
    if (zone >= 1 && zone <= 7) return ZONE_COLORS[zone - 1];
    return sf::Color(70,70,70,90);
}

int BoardView::primaryZone(const Bord& board, int space) const
{
    const std::vector<int> zones = board.getposZone(space);
    return zones.empty() ? 0 : zones.front();
}

void BoardView::drawZones(sf::RenderTarget& target, const Bord& board) const
{
    // Each logical space is tinted using its actual zone membership. Shared
    // spaces receive a neutral gold tint so the multiple-zone nature remains
    // visible without inventing a second zone system.
    for (int i = 0; i < 32; ++i)
    {
        const std::vector<int> zones = board.getposZone(i);
        if (zones.empty()) continue;

        sf::CircleShape glow(32.f);
        glow.setOrigin({32.f,32.f});
        glow.setPosition(positions[i]);

        if (zones.size() == 1)
            glow.setFillColor(zoneColor(zones.front()));
        else
            glow.setFillColor(sf::Color(165, 130, 63, 95));

        glow.setOutlineColor(sf::Color(180, 155, 105, 65));
        glow.setOutlineThickness(1.f);
        target.draw(glow);
    }
}

void BoardView::drawConnections(sf::RenderTarget& target, const Bord& board) const
{
    for (int i = 0; i < 32; ++i)
    {
        for (int j : board.getposAdjacent(i))
        {
            if (j < 0 || j >= 32 || j <= i)
                continue;

            sf::Vertex line[2];

            line[0].position = positions[i];
            line[0].color = sf::Color(154, 130, 88, 135);

            line[1].position = positions[j];
            line[1].color = sf::Color(154, 130, 88, 135);

            //target.draw(line, sf::PrimitiveType::Lines);
            target.draw(line, 2, sf::PrimitiveType::Lines);
        }
    }
}

void BoardView::draw(sf::RenderTarget& target, const Bord& board,
                     int selectedSpace, const std::vector<int>& highlightedSpaces) const
{
    sf::RectangleShape panel({700.f, 585.f});
    panel.setPosition({450.f, 88.f});
    panel.setFillColor(sf::Color(7, 10, 13, textures && textures->get("board") ? 85 : 248));
    panel.setOutlineColor(sf::Color(110, 87, 55));
    panel.setOutlineThickness(2.f);
    target.draw(panel);

    if (textures)
    {
        if (const sf::Texture* boardTexture = textures->get("board"))
        {
            sf::Sprite sprite(*boardTexture);
            const sf::Vector2u size = boardTexture->getSize();
            if (size.x > 0 && size.y > 0)
            {
                sprite.setScale({684.f / static_cast<float>(size.x),
                                 569.f / static_cast<float>(size.y)});
                sprite.setPosition({458.f, 96.f});
                target.draw(sprite);
            }
        }
    }
    else
    {
        sf::RectangleShape inner({684.f, 569.f});
        inner.setPosition({458.f, 96.f});
        inner.setFillColor(sf::Color(15, 24, 27, 255));
        inner.setOutlineColor(sf::Color(43, 66, 63));
        inner.setOutlineThickness(1.f);
        target.draw(inner);
    }

    sf::CircleShape lake(105.f);
    lake.setOrigin({105.f,105.f});
    lake.setPosition({800.f,365.f});
    lake.setFillColor(sf::Color(7, 34, 43, 170));
    lake.setOutlineColor(sf::Color(47, 79, 82, 155));
    lake.setOutlineThickness(2.f);
    target.draw(lake);

    drawZones(target, board);
    drawConnections(target, board);

    for (int i = 0; i < 32; ++i)
    {
        const std::vector<int> zones = board.getposZone(i);
        const int zone = primaryZone(board, i);

        bool highlighted = false;
        for (int h : highlightedSpaces)
            if (h == i) { highlighted = true; break; }

        sf::CircleShape space(22.f);
        space.setOrigin({22.f,22.f});
        space.setPosition(positions[i]);

        if (i == selectedSpace)
            space.setFillColor(sf::Color(205, 156, 58, 245));
        else if (highlighted)
            space.setFillColor(sf::Color(70, 130, 82, 245));
        else if (zones.size() > 1)
            space.setFillColor(sf::Color(119, 93, 48, 235));
        else
        {
            const sf::Color c = zoneColor(zone);
            space.setFillColor(sf::Color(c.r, c.g, c.b, 205));
        }

        space.setOutlineColor(sf::Color(221, 198, 149, 205));
        space.setOutlineThickness(1.6f);
        target.draw(space);

    }
}

int BoardView::getSpaceAt(sf::Vector2f mouse) const
{
    float best = 9999.f;
    int index = -1;
    for (int i = 0; i < 32; ++i)
    {
        const float dx = mouse.x - positions[i].x;
        const float dy = mouse.y - positions[i].y;
        const float d = std::sqrt(dx * dx + dy * dy);
        if (d < 30.f && d < best)
        {
            best = d;
            index = i;
        }
    }
    return index;
}

sf::Vector2f BoardView::getPosition(int space) const
{
    if (space < 0 || space >= 32) return {-100.f, -100.f};
    return positions[space];
}
