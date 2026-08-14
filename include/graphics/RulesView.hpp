#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class RulesView
{
public:
    explicit RulesView(const sf::Font& font);

    void open();
    void close();
    bool isOpen() const;

    // Returns true when the event was consumed by the Rules overlay.
    bool handleEvent(const sf::Event& event, sf::Vector2f mousePosition);
    void draw(sf::RenderTarget& target, sf::Vector2f viewportSize) const;

private:
    struct Section
    {
        std::string title;
        std::vector<std::string> paragraphs;
    };

    const sf::Font& font;
    std::vector<Section> sections;
    bool opened = false;
    std::size_t selectedSection = 0;
    float scroll = 0.f;

    static constexpr float panelWidth = 1330.f;
    static constexpr float panelHeight = 735.f;
    static constexpr float headerHeight = 72.f;
    static constexpr float footerHeight = 54.f;
    static constexpr float navigationWidth = 285.f;

    void buildSections();
    void clampScroll(float contentHeight, float viewportHeight);
    sf::FloatRect panelRect(sf::Vector2f viewportSize) const;
    sf::FloatRect contentRect(sf::Vector2f viewportSize) const;
    sf::FloatRect closeRect(sf::Vector2f viewportSize) const;

    void drawText(sf::RenderTarget& target, const std::string& text,
                  sf::Vector2f position, unsigned size, sf::Color color) const;
    float drawWrappedText(sf::RenderTarget& target, const std::string& text,
                          sf::FloatRect area, unsigned size, sf::Color color,
                          float lineSpacing = 1.32f) const;
};
