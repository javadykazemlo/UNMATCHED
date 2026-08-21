#include "graphics/CardView.hpp"
#include "cards/Card.hpp"
#include "graphics/TextureManager.hpp"

#include <algorithm>
#include <sstream>

CardView::CardView(const sf::Font& font_, const TextureManager* textures_)
    : font(font_), textures(textures_) {}

std::string CardView::textureId(const Card& card) const
{
    std::string id = "card_";
    for (char c : card.getName())
    {
        if (c >= 'A' && c <= 'Z') id += static_cast<char>(c - 'A' + 'a');
        else if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) id += c;
        else id += '_';
    }
    while (!id.empty() && id.back() == '_') id.pop_back();
    return id;
}

void CardView::drawWrappedText(sf::RenderTarget& target, const std::string& text,
                               sf::Vector2f position, unsigned size,
                               float maxWidth, sf::Color color, int maxLines) const
{
    std::istringstream words(text);
    std::string word;
    std::string line;
    int lineNo = 0;

    auto drawLine = [&](const std::string& value)
    {
        sf::Text t(font, value, size);
        t.setPosition({position.x, position.y + lineNo * (size + 3.f)});
        t.setFillColor(color);
        target.draw(t);
        ++lineNo;
    };

    while (words >> word && lineNo < maxLines)
    {
        const std::string candidate = line.empty() ? word : line + " " + word;
        sf::Text probe(font, candidate, size);
        if (probe.getLocalBounds().size.x <= maxWidth)
            line = candidate;
        else
        {
            if (!line.empty()) drawLine(line);
            line = word;
        }
    }
    if (!line.empty() && lineNo < maxLines) drawLine(line);
}

void CardView::drawCard(sf::RenderTarget& target, const Card& card,
                        sf::FloatRect rect, bool selected) const
{
    if (textures)
    {
        if (const sf::Texture* texture = textures->get(textureId(card)))
        {
            sf::Sprite sprite(*texture);
            const sf::Vector2u size = texture->getSize();
            if (size.x > 0 && size.y > 0)
            {
                sprite.setScale({rect.size.x / static_cast<float>(size.x),
                                 rect.size.y / static_cast<float>(size.y)});
                sprite.setPosition(rect.position);
                target.draw(sprite);

                if (selected)
                {
                    sf::RectangleShape outline(rect.size);
                    outline.setPosition(rect.position);
                    outline.setFillColor(sf::Color::Transparent);
                    outline.setOutlineColor(sf::Color(238, 201, 117));
                    outline.setOutlineThickness(3.f);
                    target.draw(outline);
                }
                return;
            }
        }
    }

    sf::Color accent = card.isAttack() ? sf::Color(130, 37, 40)
                     : card.isDefense() ? sf::Color(43, 76, 108)
                     : card.isScheme() ? sf::Color(91, 61, 118)
                     : sf::Color(125, 91, 42);

    sf::RectangleShape body(rect.size);
    body.setPosition(rect.position);
    body.setFillColor(sf::Color(17, 17, 23, 252));
    body.setOutlineColor(selected ? sf::Color(238, 201, 117) : accent);
    body.setOutlineThickness(selected ? 3.f : 1.5f);
    target.draw(body);

    sf::RectangleShape header({rect.size.x, 27.f});
    header.setPosition(rect.position);
    header.setFillColor(sf::Color(accent.r, accent.g, accent.b, 150));
    target.draw(header);

    sf::Text name(font, card.getName(), 10);
    name.setPosition({rect.position.x + 5.f, rect.position.y + 5.f});
    name.setFillColor(sf::Color(241, 229, 204));
    target.draw(name);

    sf::Text type(font, card.getTypeString(), 8);
    type.setPosition({rect.position.x + 5.f, rect.position.y + 31.f});
    type.setFillColor(sf::Color(177, 168, 153));
    target.draw(type);

    sf::Text owner(font, card.getOwnerString(), 7);
    owner.setPosition({rect.position.x + 5.f, rect.position.y + 46.f});
    owner.setFillColor(sf::Color(143, 137, 127));
    target.draw(owner);

    sf::Text value(font, "V " + std::to_string(card.getAttack()), 9);
    value.setPosition({rect.position.x + rect.size.x - 38.f, rect.position.y + 31.f});
    value.setFillColor(sf::Color(232, 207, 152));
    target.draw(value);

    sf::Text boost(font, "B " + std::to_string(card.getBoost()), 9);
    boost.setPosition({rect.position.x + 5.f, rect.position.y + rect.size.y - 30.f});
    boost.setFillColor(sf::Color(208, 183, 126));
    target.draw(boost);

    sf::Text timing(font, card.getTimingString(), 7);
    timing.setPosition({rect.position.x + 5.f, rect.position.y + rect.size.y - 17.f});
    timing.setFillColor(sf::Color(143, 137, 127));
    target.draw(timing);

    drawWrappedText(target, card.geteffect(),
                    {rect.position.x + 5.f, rect.position.y + 60.f},
                    7, rect.size.x - 10.f, sf::Color(196, 190, 177), 7);
}

void CardView::drawHand(sf::RenderTarget& target, const std::vector<Card>& cards,
                        int selectedIndex) const
{
    const float x = 525.f;
    const float y = 714.f;
    const float w = 93.f;
    const float h = 145.f;
    const float gap = 5.f;

    const int visible = std::min(7, static_cast<int>(cards.size()));
    for (int i = 0; i < visible; ++i)
    {
        sf::FloatRect rect({x + i * (w + gap), y}, {w, h});
        if (i == selectedIndex) rect.position.y -= 12.f;
        drawCard(target, cards[i], rect, i == selectedIndex);
    }
}

int CardView::getCardAt(sf::Vector2f mouse, int cardCount) const
{
    const float x = 525.f;
    const float y = 714.f;
    const float w = 93.f;
    const float h = 145.f;
    const float gap = 5.f;

    for (int i = 0; i < std::min(7, cardCount); ++i)
    {
        sf::FloatRect rect({x + i * (w + gap), y}, {w, h});
        if (rect.contains(mouse)) return i;
    }
    return -1;
}
