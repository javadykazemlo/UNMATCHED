#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────
//  UI.hpp
//  Small, dependency-free SFML widget toolkit used by every other graphics/
//  file (BoardView, CardView, CharacterView, DeckView, GameWindow).
//  Everything here is purely visual / input-capture: it never touches the
//  game rules (Controller / Bord / Player / Deck / Card / Character).
// ─────────────────────────────────────────────────────────────────────────

namespace Theme
{
    // Gothic / "Cobble & Fog" inspired palette --------------------------------
    extern const sf::Color Background;
    extern const sf::Color BackgroundLight;
    extern const sf::Color PanelBg;
    extern const sf::Color PanelBgSoft;
    extern const sf::Color PanelBorder;
    extern const sf::Color Gold;
    extern const sf::Color GoldBright;
    extern const sf::Color TextLight;
    extern const sf::Color TextMuted;
    extern const sf::Color Player1;      // Dracula red
    extern const sf::Color Player2;      // Sherlock blue
    extern const sf::Color Danger;
    extern const sf::Color Success;
    extern const sf::Color AttackCard;
    extern const sf::Color DefenseCard;
    extern const sf::Color VersatileCard;
    extern const sf::Color SchemeCard;

    const sf::Font& titleFont();   // Cinzel-Bold.ttf
    const sf::Font& bodyFont();    // same family, used for body text too

    void centerOrigin(sf::Text& text);
    void centerOrigin(sf::RectangleShape& shape);
    std::string wordWrap(const sf::Font& font, const std::string& text,
                          unsigned int charSize, float maxWidth);
}

// ── Panel ───────────────────────────────────────────────────────────────
// A translucent, gold-bordered rectangle used as a background for every
// grouping of widgets (side panels, action panel, hand panel, dialogs...).
class Panel
{
public:
    Panel(sf::Vector2f position, sf::Vector2f size,
          sf::Color fill = Theme::PanelBg, sf::Color border = Theme::PanelBorder);

    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    sf::FloatRect getBounds() const;

    void setTitle(const std::string& title);
    void draw(sf::RenderTarget& target) const;

private:
    sf::RectangleShape box_;
    sf::RectangleShape titleBar_;
    sf::Text title_;
    bool hasTitle_ = false;
};

// ── Button ──────────────────────────────────────────────────────────────
class Button
{
public:
    Button(const std::string& label, sf::Vector2f position, sf::Vector2f size,
           sf::Color base = Theme::PanelBgSoft, sf::Color accent = Theme::Gold);

    void setLabel(const std::string& label);
    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void setColors(sf::Color base, sf::Color accent);
    void setEnabled(bool enabled);
    void setSelected(bool selected);
    bool isEnabled() const { return enabled_; }
    sf::FloatRect getBounds() const;

    // call every frame with the mouse position (world/view coordinates)
    void updateHover(sf::Vector2f mousePos);
    // call on a mouse-released event; returns true exactly once when clicked
    bool click(sf::Vector2f mousePos);

    void draw(sf::RenderTarget& target) const;

private:
    sf::RectangleShape box_;
    sf::Text label_;
    sf::Color base_, accent_;
    bool enabled_ = true;
    bool hovered_ = false;
    bool selected_ = false;
};

// ── Label (auto word-wrapped block of text) ────────────────────────────
class Label
{
public:
    Label(const std::string& text = "", unsigned int charSize = 18,
          sf::Color color = Theme::TextLight, bool bold = false);

    void setText(const std::string& text);
    void setPosition(sf::Vector2f pos);
    void setColor(sf::Color color);
    void setCharacterSize(unsigned int size);
    void setWrapWidth(float width); // 0 = no wrap
    sf::FloatRect getBounds() const;

    void draw(sf::RenderTarget& target) const;

private:
    sf::Text text_;
    std::string raw_;
    float wrapWidth_ = 0.f;
    void rebuild();
};

// ── HP / progress bar ──────────────────────────────────────────────────
class ProgressBar
{
public:
    ProgressBar(sf::Vector2f position, sf::Vector2f size);

    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void setValue(int current, int max);
    void draw(sf::RenderTarget& target) const;

private:
    sf::RectangleShape back_;
    sf::RectangleShape fill_;
    sf::Text text_;
    sf::Vector2f pos_, size_;
    int current_ = 0, max_ = 1;
    void layout();
};

// ── Single line editable text field (names, free text) ────────────────
class TextField
{
public:
    TextField(sf::Vector2f position, sf::Vector2f size, const std::string& placeholder = "");

    void setPosition(sf::Vector2f pos);
    void setFocused(bool focused);
    bool isFocused() const { return focused_; }
    void handleTextEntered(std::uint32_t unicode);
    void handleClick(sf::Vector2f mousePos); // focuses if inside
    const std::string& getValue() const { return value_; }
    void setValue(const std::string& v) { value_ = v; }
    sf::FloatRect getBounds() const;

    void draw(sf::RenderTarget& target) const;

private:
    sf::RectangleShape box_;
    sf::Text text_;
    sf::Text placeholderText_;
    std::string value_;
    std::string placeholder_;
    bool focused_ = false;
};

// ── Numeric spin box (used for the age inputs) ─────────────────────────
class SpinBox
{
public:
    SpinBox(sf::Vector2f position, sf::Vector2f size, int minV = 1, int maxV = 120, int value = 25);

    void setPosition(sf::Vector2f pos);
    int getValue() const { return value_; }
    void setValue(int v);

    void updateHover(sf::Vector2f mousePos);
    // returns true if the value changed this click
    bool click(sf::Vector2f mousePos);

    void draw(sf::RenderTarget& target) const;

private:
    sf::RectangleShape box_;
    sf::RectangleShape upBtn_, downBtn_;
    sf::Text valueText_, upText_, downText_;
    int minV_, maxV_, value_;
};

// ── Small pill / chip (used for zone tags, counters, log entries) ─────
class Chip
{
public:
    Chip(const std::string& text, sf::Vector2f position, sf::Color color = Theme::Gold);
    void setPosition(sf::Vector2f pos);
    void draw(sf::RenderTarget& target) const;
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape box_;
    sf::Text text_;
};