#include "graphics/CardView.hpp"
#include "cards/Card.hpp"
#include <algorithm>

CardView::CardView(const sf::Font& font) : font(font) {}

void CardView::drawCard(sf::RenderTarget& target, const Card& card,
                        sf::FloatRect rect, bool selected) const
{
    sf::RectangleShape body(rect.size);
    body.setPosition(rect.position);

    sf::Color accent = card.isAttack() ? sf::Color(130, 37, 37)
                      : card.isDefense() ? sf::Color(48, 76, 105)
                      : card.isScheme() ? sf::Color(94, 64, 120)
                      : sf::Color(111, 84, 45);

    body.setFillColor(sf::Color(20, 19, 25, 250));
    body.setOutlineColor(selected ? sf::Color(230, 193, 112) : accent);
    body.setOutlineThickness(selected ? 3.f : 1.5f);
    target.draw(body);

    sf::RectangleShape header({rect.size.x, 24.f});
    header.setPosition(rect.position);
    header.setFillColor(sf::Color(accent.r, accent.g, accent.b, 115));
    target.draw(header);

    sf::Text name(font, card.getName(), 11);
    name.setPosition({rect.position.x + 6.f, rect.position.y + 5.f});
    name.setFillColor(sf::Color(238, 226, 199));
    target.draw(name);

    sf::Text type(font, card.getTypeString(), 9);
    type.setPosition({rect.position.x + 6.f, rect.position.y + 29.f});
    type.setFillColor(sf::Color(164, 156, 145));
    target.draw(type);

    sf::Text value(font, std::to_string(card.getAttack()), 22);
    value.setPosition({rect.position.x + rect.size.x - 28.f, rect.position.y + 25.f});
    value.setFillColor(sf::Color(232, 207, 152));
    target.draw(value);

    sf::Text boost(font, "+" + std::to_string(card.getBoost()), 10);
    boost.setPosition({rect.position.x + 7.f, rect.position.y + rect.size.y - 20.f});
    boost.setFillColor(sf::Color(196, 176, 131));
    target.draw(boost);
}

void CardView::drawHand(sf::RenderTarget& target, const std::vector<Card>& cards,
                        int selectedIndex) const
{
    if (cards.empty())
        return;

    float x = 565.f;
    float y = 706.f;
    float w = 112.f;
    float h = 145.f;
    float gap = 8.f;

    for (int i = 0; i < static_cast<int>(cards.size()) && i < 5; ++i)
    {
        sf::FloatRect r({x + i*(w+gap), y}, {w,h});
        if (i == selectedIndex)
            r.position.y -= 12.f;
        drawCard(target, cards[i], r, i == selectedIndex);
    }
}

int CardView::getCardAt(sf::Vector2f mouse, int cardCount) const
{
    float x = 565.f;
    float y = 706.f;
    float w = 112.f;
    float h = 145.f;
    float gap = 8.f;

    for (int i = 0; i < cardCount && i < 5; ++i)
    {
        sf::FloatRect r({x + i*(w+gap), y - (i == 0 ? 0.f : 0.f)}, {w,h});
        if (r.contains(mouse))
            return i;
    }
    return -1;
}
