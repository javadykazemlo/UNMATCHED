#include "graphics/BoardView.hpp"
#include "graphics/UI.hpp"
#include <algorithm>
#include <cmath>

BoardView::BoardView(sf::Vector2f position, sf::Vector2f size)
    : position_(position), size_(size)
{
}

void BoardView::setPosition(sf::Vector2f pos) { position_ = pos; }
void BoardView::setSize(sf::Vector2f size) { size_ = size; }
void BoardView::setBoard(Bord* board) { board_ = board; }
void BoardView::setHighlighted(const std::vector<int>& spaces) { highlighted_ = spaces; }
void BoardView::setSelected(int space) { selected_ = space; }

// Hand-authored layout that mirrors the shape of Bord's adjacency graph
// (two starting rooms at 4 and 15, four secret-passage rooms at the
// corners: 0, 12, 17, 23). Values are fractions of the panel size.
sf::Vector2f BoardView::nodeLayoutFraction(int i)
{
    static const sf::Vector2f layout[32] = {
        {0.10f, 0.16f}, {0.05f, 0.32f}, {0.20f, 0.08f}, {0.30f, 0.14f},
        {0.16f, 0.44f}, {0.30f, 0.34f}, {0.20f, 0.56f}, {0.28f, 0.64f},
        {0.14f, 0.66f}, {0.30f, 0.74f}, {0.42f, 0.80f}, {0.40f, 0.66f},
        {0.50f, 0.90f}, {0.55f, 0.62f}, {0.65f, 0.56f}, {0.80f, 0.50f},
        {0.45f, 0.56f}, {0.55f, 0.40f}, {0.62f, 0.28f}, {0.72f, 0.26f},
        {0.38f, 0.24f}, {0.35f, 0.18f}, {0.55f, 0.16f}, {0.66f, 0.08f},
        {0.76f, 0.06f}, {0.84f, 0.14f}, {0.72f, 0.20f}, {0.82f, 0.24f},
        {0.62f, 0.44f}, {0.70f, 0.47f}, {0.80f, 0.63f}, {0.68f, 0.63f}
    };
    if (i < 0 || i >= 32) return {0.5f, 0.5f};
    return layout[i];
}

sf::Vector2f BoardView::nodePixelPos(int index) const
{
    sf::Vector2f f = nodeLayoutFraction(index);
    float pad = 30.f;
    return {
        position_.x + pad + f.x * (size_.x - 2.f * pad),
        position_.y + pad + f.y * (size_.y - 2.f * pad)
    };
}

bool BoardView::isSecretPassage(int index) const
{
    return index == 0 || index == 12 || index == 17 || index == 23;
}

void BoardView::updateHover(sf::Vector2f mousePos)
{
    hoveredNode_ = -1;
    for (int i = 0; i < 32; ++i)
    {
        sf::Vector2f p = nodePixelPos(i);
        sf::Vector2f d = mousePos - p;
        if (d.x * d.x + d.y * d.y <= nodeRadius_ * nodeRadius_)
        {
            hoveredNode_ = i;
            break;
        }
    }
}

int BoardView::click(sf::Vector2f mousePos) const
{
    for (int i = 0; i < 32; ++i)
    {
        sf::Vector2f p = nodePixelPos(i);
        sf::Vector2f d = mousePos - p;
        if (d.x * d.x + d.y * d.y <= nodeRadius_ * nodeRadius_)
            return i;
    }
    return -1;
}

void BoardView::draw(sf::RenderTarget& target) const
{
    if (!board_) return;

    // panel background --------------------------------------------------------
    sf::RectangleShape bg(size_);
    bg.setPosition(position_);
    bg.setFillColor(sf::Color(14, 12, 18));
    bg.setOutlineColor(Theme::PanelBorder);
    bg.setOutlineThickness(2.f);
    target.draw(bg);

    // edges ---------------------------------------------------------------------
    sf::VertexArray lines(sf::PrimitiveType::Lines);
    for (int i = 0; i < 32; ++i)
    {
        for (int n : board_->getposAdjacent(i))
        {
            if (n <= i) continue; // draw each edge once
            sf::Vertex a, b;
            a.position = nodePixelPos(i);
            b.position = nodePixelPos(n);
            a.color = b.color = sf::Color(90, 80, 70, 160);
            lines.append(a);
            lines.append(b);
        }
    }
    target.draw(lines);

    // secret passage web (dotted gold lines between the 4 tunnel rooms) --------
    static const int secret[4] = {0, 12, 17, 23};
    for (int a = 0; a < 4; ++a)
    {
        for (int b = a + 1; b < 4; ++b)
        {
            sf::Vector2f p1 = nodePixelPos(secret[a]);
            sf::Vector2f p2 = nodePixelPos(secret[b]);
            sf::Vector2f dir = p2 - p1;
            float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            if (len < 1.f) continue;
            dir /= len;
            for (float t = 0.f; t < len; t += 14.f)
            {
                sf::CircleShape dot(1.6f);
                dot.setFillColor(sf::Color(196, 164, 86, 120));
                dot.setPosition(p1 + dir * t - sf::Vector2f(1.6f, 1.6f));
                target.draw(dot);
            }
        }
    }

    // nodes ----------------------------------------------------------------------
    for (int i = 0; i < 32; ++i)
    {
        sf::Vector2f p = nodePixelPos(i);
        Character* occupant = board_->getCharacter(i);
        bool isHighlighted = std::find(highlighted_.begin(), highlighted_.end(), i) != highlighted_.end();
        bool isHovered = (hoveredNode_ == i);
        bool isSelected = (selected_ == i);

        if (occupant)
        {
            CharacterView cv(p, nodeRadius_ + (isHovered ? 3.f : 0.f));
            cv.setCharacter(occupant);
            cv.setCompact(true);
            cv.setSelected(isSelected);
            cv.draw(target);
        }
        else
        {
            sf::CircleShape node(nodeRadius_);
            node.setOrigin({ nodeRadius_, nodeRadius_ });
            node.setPosition(p);

            if (isSelected)
            {
                node.setFillColor(Theme::GoldBright);
                node.setOutlineColor(sf::Color::White);
            }
            else if (isHighlighted)
            {
                node.setFillColor(sf::Color(60, 130, 80, 220));
                node.setOutlineColor(Theme::Success);
            }
            else if (isSecretPassage(i))
            {
                node.setFillColor(sf::Color(40, 30, 45));
                node.setOutlineColor(Theme::Gold);
            }
            else
            {
                node.setFillColor(sf::Color(30, 26, 34));
                node.setOutlineColor(sf::Color(110, 100, 90));
            }
            node.setOutlineThickness(isHovered ? 3.f : 2.f);
            target.draw(node);

            sf::Text label(Theme::bodyFont(), std::to_string(i), 11);
            label.setFillColor(isHighlighted || isSelected ? sf::Color::Black : Theme::TextMuted);
            Theme::centerOrigin(label);
            label.setPosition(p);
            target.draw(label);
        }
    }

    // legend -----------------------------------------------------------------
    sf::Text legend(Theme::bodyFont(),
                     "gold ring = secret passage    green = available    dotted line = tunnel network", 11);
    legend.setFillColor(Theme::TextMuted);
    legend.setPosition({ position_.x + 10.f, position_.y + size_.y - 18.f });
    target.draw(legend);
}