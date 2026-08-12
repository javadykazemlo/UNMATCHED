#include "graphics/UI.hpp"
#include <algorithm>

UI::UI(const sf::Font& font) : font(font) {}

void UI::drawPanel(sf::RenderTarget& target, sf::FloatRect rect, sf::Color border) const
{
    sf::RectangleShape outer(rect.size);
    outer.setPosition(rect.position);
    outer.setFillColor(sf::Color(10, 11, 17, 238));
    outer.setOutlineColor(border);
    outer.setOutlineThickness(1.5f);
    target.draw(outer);

    sf::RectangleShape inner({rect.size.x - 8.f, rect.size.y - 8.f});
    inner.setPosition({rect.position.x + 4.f, rect.position.y + 4.f});
    inner.setFillColor(sf::Color(20, 20, 29, 150));
    inner.setOutlineColor(sf::Color(border.r, border.g, border.b, 80));
    inner.setOutlineThickness(1.f);
    target.draw(inner);
}

void UI::drawText(sf::RenderTarget& target, const std::string& text,
                  sf::Vector2f position, unsigned size, sf::Color color) const
{
    sf::Text t(font, text, size);
    t.setPosition(position);
    t.setFillColor(color);
    target.draw(t);
}

void UI::drawButton(sf::RenderTarget& target, sf::FloatRect rect,
                    const std::string& label, bool active,
                    sf::Color accent) const
{
    sf::RectangleShape button(rect.size);
    button.setPosition(rect.position);
    button.setFillColor(active ? sf::Color(58, 13, 18, 245)
                                : sf::Color(18, 18, 27, 245));
    button.setOutlineColor(active ? accent : sf::Color(75, 68, 60));
    button.setOutlineThickness(active ? 2.f : 1.f);
    target.draw(button);

    sf::Text text(font, label, 18);
    auto bounds = text.getLocalBounds();
    text.setOrigin({bounds.position.x + bounds.size.x / 2.f,
                    bounds.position.y + bounds.size.y / 2.f});
    text.setPosition({rect.position.x + rect.size.x / 2.f, 
                      rect.position.y + rect.size.y / 2.f});
    text.setFillColor(active ? sf::Color(240, 225, 190) : sf::Color(175, 168, 158));
    target.draw(text);
}

void UI::drawHealth(sf::RenderTarget& target, sf::Vector2f pos,
                    float ratio, float width, sf::Color accent) const
{
    ratio = std::clamp(ratio, 0.f, 1.f);

    sf::RectangleShape bg({width, 12.f});
    bg.setPosition(pos);
    bg.setFillColor(sf::Color(35, 34, 38));
    bg.setOutlineColor(sf::Color(80, 75, 70));
    bg.setOutlineThickness(1.f);
    target.draw(bg);

    sf::RectangleShape fill({width * ratio, 12.f});
    fill.setPosition(pos);
    fill.setFillColor(accent);
    target.draw(fill);
}
