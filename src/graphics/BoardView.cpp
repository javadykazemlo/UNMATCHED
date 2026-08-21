#include "graphics/BoardView.hpp"
#include "core/Bord.hpp"
#include "entities/Character.hpp"
#include "graphics/TextureManager.hpp"

#include <cmath>

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
    positions = {
        sf::Vector2f{517.f,175.f},
        /*1*/{600.f,170.f}, {503.f,268.f},
        /*3*/{580.f,275.f}, {658.f,237.f}, {678.f,318.f}, {728.f,174.f},/*6*/
        /*7*/{801.f,229.f}, {854.f,174.f}, {934.f,232.f}, {1000.f,178.f}, {1045.f,230.f},/*11*/
        /*12*/{1088.f,179.f}, {1035.f,317.f}, {1092.f,379.f}, {992.f,405.f}, {927.f,367.f},/*16*/
        /*17*/{831.f,333.f}, {766.f,378.f}, {680.f,422.f}, {601.f,352.f}, {517.f,368.f},/*21*/
        /*22*/{568.f,458.f}, {607.f,527.f}, {686.f,521.f}, {758.f,527.f}, {803.f,468.f},/*26*/
        /*27*/{838.f,532.f}, {895.f,486.f}, {975.f,489.f},
        /*30*/{1042.f,540.f}, {1092.f,488.f}
    };
}

sf::Color BoardView::zoneColor(int zone) const
{
    if (zone >= 1 && zone <= 7)
        return ZONE_COLORS[zone - 1];
    return sf::Color(70, 70, 70, 90);
}

int BoardView::primaryZone(const Bord& board, int space) const
{
    const std::vector<int> zones = board.getposZone(space);
    return zones.empty() ? 0 : zones.front();
}

void BoardView::drawZones(sf::RenderTarget& target, const Bord& board) const
{
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

    const bool hasBoardArt = textures && textures->get("board") != nullptr;

    if (!hasBoardArt)
    {
        sf::CircleShape lake(105.f);
        lake.setOrigin({105.f,105.f});
        lake.setPosition({800.f,365.f});
        lake.setFillColor(sf::Color(7, 34, 43, 170));
        lake.setOutlineColor(sf::Color(47, 79, 82, 155));
        lake.setOutlineThickness(2.f);
        target.draw(lake);

        drawZones(target, board);
        drawConnections(target, board);
    }

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
        else if (hasBoardArt)
            space.setFillColor(sf::Color(0, 0, 0, 0));
        else if (zones.size() > 1)
            space.setFillColor(sf::Color(119, 93, 48, 235));
        else
        {
            const sf::Color c = zoneColor(zone);
            space.setFillColor(sf::Color(c.r, c.g, c.b, 205));
        }

        space.setOutlineColor(hasBoardArt && i != selectedSpace && !highlighted
                                   ? sf::Color(221, 198, 149, 90)
                                   : sf::Color(221, 198, 149, 205));
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
