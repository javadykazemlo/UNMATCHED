#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class UI
{
private:
    const sf::Font& font;
public:
    explicit UI(const sf::Font& font);
    void drawPanel(sf::RenderTarget& target, sf::FloatRect rect, sf::Color border) const;
    void drawText(sf::RenderTarget& target, const std::string& text,
                  sf::Vector2f position, unsigned size, sf::Color color = sf::Color::White) const;
    void drawButton(sf::RenderTarget& target, sf::FloatRect rect,
                    const std::string& label, bool active, sf::Color accent) const;
    void drawHealth(sf::RenderTarget& target, sf::Vector2f pos,
                    float ratio, float width, sf::Color accent) const;
};