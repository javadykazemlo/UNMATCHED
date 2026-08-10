#include "graphics/UI.hpp"
#include "graphics/TextureManager.hpp"
#include <sstream>

// ── Theme ───────────────────────────────────────────────────────────────
namespace Theme
{
    const sf::Color Background      (12, 10, 16);
    const sf::Color BackgroundLight (24, 20, 28);
    const sf::Color PanelBg         (18, 14, 22, 235);
    const sf::Color PanelBgSoft     (30, 24, 34, 235);
    const sf::Color PanelBorder     (196, 164, 86);
    const sf::Color Gold            (212, 175, 90);
    const sf::Color GoldBright      (238, 210, 140);
    const sf::Color TextLight       (228, 222, 214);
    const sf::Color TextMuted       (150, 142, 140);
    const sf::Color Player1         (150, 30, 40);
    const sf::Color Player2         (55, 100, 160);
    const sf::Color Danger          (176, 40, 40);
    const sf::Color Success         (70, 150, 90);
    const sf::Color AttackCard      (150, 45, 45);
    const sf::Color DefenseCard     (55, 95, 155);
    const sf::Color VersatileCard   (125, 75, 155);
    const sf::Color SchemeCard      (65, 125, 75);

    const sf::Font& titleFont() { return TextureManager::instance().titleFont(); }
    const sf::Font& bodyFont()  { return TextureManager::instance().bodyFont(); }

    void centerOrigin(sf::Text& text)
    {
        sf::FloatRect b = text.getLocalBounds();
        text.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f });
    }

    void centerOrigin(sf::RectangleShape& shape)
    {
        sf::FloatRect b = shape.getLocalBounds();
        shape.setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
    }

    std::string wordWrap(const sf::Font& font, const std::string& text,
                          unsigned int charSize, float maxWidth)
    {
        if (maxWidth <= 0.f) return text;

        std::istringstream words(text);
        std::string word, line, out;
        sf::Text probe(font, "", charSize);

        while (words >> word)
        {
            std::string tentative = line.empty() ? word : line + " " + word;
            probe.setString(tentative);
            if (probe.getLocalBounds().size.x > maxWidth && !line.empty())
            {
                out += line + "\n";
                line = word;
            }
            else
            {
                line = tentative;
            }
        }
        if (!line.empty()) out += line;
        return out;
    }
}

// ── Panel ───────────────────────────────────────────────────────────────
Panel::Panel(sf::Vector2f position, sf::Vector2f size, sf::Color fill, sf::Color border)
    : box_(size), titleBar_(sf::Vector2f(size.x, 30.f)), title_(Theme::titleFont(), "", 16)
{
    box_.setPosition(position);
    box_.setFillColor(fill);
    box_.setOutlineColor(border);
    box_.setOutlineThickness(2.f);

    titleBar_.setPosition(position);
    titleBar_.setFillColor(sf::Color(0, 0, 0, 90));

    title_.setFillColor(Theme::GoldBright);
    title_.setStyle(sf::Text::Bold);
}

void Panel::setPosition(sf::Vector2f pos)
{
    box_.setPosition(pos);
    titleBar_.setPosition(pos);
    title_.setPosition({ pos.x + 12.f, pos.y + 5.f });
}

void Panel::setSize(sf::Vector2f size)
{
    box_.setSize(size);
    titleBar_.setSize({ size.x, 30.f });
}

sf::Vector2f Panel::getPosition() const { return box_.getPosition(); }
sf::Vector2f Panel::getSize() const { return box_.getSize(); }
sf::FloatRect Panel::getBounds() const { return box_.getGlobalBounds(); }

void Panel::setTitle(const std::string& t)
{
    hasTitle_ = !t.empty();
    title_.setString(t);
    title_.setPosition({ box_.getPosition().x + 12.f, box_.getPosition().y + 5.f });
}

void Panel::draw(sf::RenderTarget& target) const
{
    target.draw(box_);
    if (hasTitle_)
    {
        target.draw(titleBar_);
        target.draw(title_);
    }
}

// ── Button ──────────────────────────────────────────────────────────────
Button::Button(const std::string& label, sf::Vector2f position, sf::Vector2f size,
               sf::Color base, sf::Color accent)
    : box_(size), label_(Theme::titleFont(), label, 16), base_(base), accent_(accent)
{
    box_.setPosition(position);
    box_.setFillColor(base_);
    box_.setOutlineColor(accent_);
    box_.setOutlineThickness(2.f);

    label_.setFillColor(Theme::TextLight);
    Theme::centerOrigin(label_);
    label_.setPosition({ position.x + size.x / 2.f, position.y + size.y / 2.f - 2.f });
}

void Button::setLabel(const std::string& text)
{
    label_.setString(text);
    Theme::centerOrigin(label_);
    sf::Vector2f p = box_.getPosition();
    sf::Vector2f s = box_.getSize();
    label_.setPosition({ p.x + s.x / 2.f, p.y + s.y / 2.f - 2.f });
}

void Button::setPosition(sf::Vector2f pos)
{
    box_.setPosition(pos);
    sf::Vector2f s = box_.getSize();
    label_.setPosition({ pos.x + s.x / 2.f, pos.y + s.y / 2.f - 2.f });
}

void Button::setSize(sf::Vector2f size)
{
    box_.setSize(size);
    sf::Vector2f p = box_.getPosition();
    label_.setPosition({ p.x + size.x / 2.f, p.y + size.y / 2.f - 2.f });
}

void Button::setColors(sf::Color base, sf::Color accent)
{
    base_ = base; accent_ = accent;
    box_.setFillColor(base_);
    box_.setOutlineColor(accent_);
}

void Button::setEnabled(bool enabled) { enabled_ = enabled; }
void Button::setSelected(bool selected) { selected_ = selected; }
sf::FloatRect Button::getBounds() const { return box_.getGlobalBounds(); }

void Button::updateHover(sf::Vector2f mousePos)
{
    hovered_ = enabled_ && box_.getGlobalBounds().contains(mousePos);
}

bool Button::click(sf::Vector2f mousePos)
{
    if (!enabled_) return false;
    return box_.getGlobalBounds().contains(mousePos);
}

void Button::draw(sf::RenderTarget& target) const
{
    sf::RectangleShape drawBox = box_;
    if (!enabled_)
    {
        drawBox.setFillColor(sf::Color(30, 28, 30, 160));
        drawBox.setOutlineColor(sf::Color(90, 85, 80));
    }
    else if (selected_)
    {
        drawBox.setFillColor(accent_);
        drawBox.setOutlineColor(Theme::GoldBright);
        drawBox.setOutlineThickness(3.f);
    }
    else if (hovered_)
    {
        drawBox.setFillColor(sf::Color(base_.r + 20, base_.g + 20, base_.b + 25, base_.a));
        drawBox.setOutlineColor(Theme::GoldBright);
    }
    target.draw(drawBox);

    sf::Text drawLabel = label_;
    drawLabel.setFillColor(enabled_ ? (selected_ ? sf::Color::Black : Theme::TextLight)
                                     : Theme::TextMuted);
    target.draw(drawLabel);
}

// ── Label ───────────────────────────────────────────────────────────────
Label::Label(const std::string& text, unsigned int charSize, sf::Color color, bool bold)
    : text_(Theme::bodyFont(), "", charSize), raw_(text)
{
    text_.setFillColor(color);
    if (bold) text_.setStyle(sf::Text::Bold);
    rebuild();
}

void Label::setText(const std::string& text) { raw_ = text; rebuild(); }
void Label::setPosition(sf::Vector2f pos) { text_.setPosition(pos); }
void Label::setColor(sf::Color color) { text_.setFillColor(color); }
void Label::setCharacterSize(unsigned int size) { text_.setCharacterSize(size); rebuild(); }
void Label::setWrapWidth(float width) { wrapWidth_ = width; rebuild(); }
sf::FloatRect Label::getBounds() const { return text_.getGlobalBounds(); }

void Label::rebuild()
{
    if (wrapWidth_ > 0.f)
        text_.setString(Theme::wordWrap(text_.getFont(), raw_, text_.getCharacterSize(), wrapWidth_));
    else
        text_.setString(raw_);
}

void Label::draw(sf::RenderTarget& target) const { target.draw(text_); }

// ── ProgressBar ─────────────────────────────────────────────────────────
ProgressBar::ProgressBar(sf::Vector2f position, sf::Vector2f size)
    : back_(size), fill_(size), text_(Theme::bodyFont(), "", 14), pos_(position), size_(size)
{
    back_.setPosition(position);
    back_.setFillColor(sf::Color(35, 20, 22));
    back_.setOutlineColor(Theme::PanelBorder);
    back_.setOutlineThickness(1.f);

    fill_.setPosition(position);
    fill_.setFillColor(Theme::Success);

    text_.setFillColor(Theme::TextLight);
}

void ProgressBar::setPosition(sf::Vector2f pos) { pos_ = pos; layout(); }
void ProgressBar::setSize(sf::Vector2f size) { size_ = size; layout(); }

void ProgressBar::setValue(int current, int max)
{
    current_ = current; max_ = max > 0 ? max : 1;
    layout();
}

void ProgressBar::layout()
{
    back_.setPosition(pos_);
    back_.setSize(size_);

    float ratio = max_ > 0 ? std::max(0.f, std::min(1.f, float(current_) / float(max_))) : 0.f;
    fill_.setPosition(pos_);
    fill_.setSize({ size_.x * ratio, size_.y });

    if (ratio > 0.5f) fill_.setFillColor(Theme::Success);
    else if (ratio > 0.25f) fill_.setFillColor(sf::Color(200, 160, 40));
    else fill_.setFillColor(Theme::Danger);

    std::string s = std::to_string(current_) + " / " + std::to_string(max_);
    text_.setString(s);
    Theme::centerOrigin(text_);
    text_.setPosition({ pos_.x + size_.x / 2.f, pos_.y + size_.y / 2.f - 1.f });
}

void ProgressBar::draw(sf::RenderTarget& target) const
{
    target.draw(back_);
    target.draw(fill_);
    target.draw(text_);
}

// ── TextField ──────────────────────────────────────────────────────────
TextField::TextField(sf::Vector2f position, sf::Vector2f size, const std::string& placeholder)
    : box_(size), text_(Theme::bodyFont(), "", 18), placeholderText_(Theme::bodyFont(), placeholder, 18),
      placeholder_(placeholder)
{
    box_.setPosition(position);
    box_.setFillColor(sf::Color(20, 16, 24));
    box_.setOutlineColor(Theme::PanelBorder);
    box_.setOutlineThickness(2.f);

    text_.setFillColor(Theme::TextLight);
    text_.setPosition({ position.x + 10.f, position.y + size.y / 2.f - 12.f });

    placeholderText_.setFillColor(Theme::TextMuted);
    placeholderText_.setPosition({ position.x + 10.f, position.y + size.y / 2.f - 12.f });
}

void TextField::setPosition(sf::Vector2f pos)
{
    box_.setPosition(pos);
    text_.setPosition({ pos.x + 10.f, pos.y + box_.getSize().y / 2.f - 12.f });
    placeholderText_.setPosition({ pos.x + 10.f, pos.y + box_.getSize().y / 2.f - 12.f });
}

void TextField::setFocused(bool focused) { focused_ = focused; }

void TextField::handleTextEntered(std::uint32_t unicode)
{
    if (!focused_) return;
    if (unicode == 8) // backspace
    {
        if (!value_.empty()) value_.pop_back();
    }
    else if (unicode == 13 || unicode == 10)
    {
        // enter: handled by the owning screen
    }
    else if (unicode >= 32 && unicode < 127 && value_.size() < 40)
    {
        value_ += static_cast<char>(unicode);
    }
}

void TextField::handleClick(sf::Vector2f mousePos)
{
    focused_ = box_.getGlobalBounds().contains(mousePos);
}

sf::FloatRect TextField::getBounds() const { return box_.getGlobalBounds(); }

void TextField::draw(sf::RenderTarget& target) const
{
    sf::RectangleShape drawBox = box_;
    drawBox.setOutlineColor(focused_ ? Theme::GoldBright : Theme::PanelBorder);
    target.draw(drawBox);

    if (value_.empty() && !focused_)
    {
        target.draw(placeholderText_);
    }
    else
    {
        sf::Text drawText = text_;
        std::string shown = value_ + (focused_ ? "_" : "");
        drawText.setString(shown);
        target.draw(drawText);
    }
}

// ── SpinBox ────────────────────────────────────────────────────────────
SpinBox::SpinBox(sf::Vector2f position, sf::Vector2f size, int minV, int maxV, int value)
    : box_(size), upBtn_(sf::Vector2f(size.y * 0.8f, size.y / 2.f - 2.f)),
      downBtn_(sf::Vector2f(size.y * 0.8f, size.y / 2.f - 2.f)),
      valueText_(Theme::titleFont(), "", 22),
      upText_(Theme::bodyFont(), "+", 16),
      downText_(Theme::bodyFont(), "-", 18),
      minV_(minV), maxV_(maxV), value_(value)
{
    box_.setPosition(position);
    box_.setFillColor(sf::Color(20, 16, 24));
    box_.setOutlineColor(Theme::PanelBorder);
    box_.setOutlineThickness(2.f);

    upBtn_.setFillColor(sf::Color(40, 34, 30));
    downBtn_.setFillColor(sf::Color(40, 34, 30));

    valueText_.setFillColor(Theme::GoldBright);
    upText_.setFillColor(Theme::TextLight);
    downText_.setFillColor(Theme::TextLight);

    setPosition(position);
}

void SpinBox::setPosition(sf::Vector2f pos)
{
    sf::Vector2f size = box_.getSize();
    box_.setPosition(pos);

    float btnW = upBtn_.getSize().x;
    upBtn_.setPosition({ pos.x + size.x - btnW - 4.f, pos.y + 2.f });
    downBtn_.setPosition({ pos.x + size.x - btnW - 4.f, pos.y + size.y / 2.f + 2.f });

    valueText_.setString(std::to_string(value_));
    Theme::centerOrigin(valueText_);
    valueText_.setPosition({ pos.x + (size.x - btnW) / 2.f, pos.y + size.y / 2.f - 2.f });

    Theme::centerOrigin(upText_);
    upText_.setPosition({ upBtn_.getPosition().x + upBtn_.getSize().x / 2.f,
                           upBtn_.getPosition().y + upBtn_.getSize().y / 2.f - 2.f });
    Theme::centerOrigin(downText_);
    downText_.setPosition({ downBtn_.getPosition().x + downBtn_.getSize().x / 2.f,
                             downBtn_.getPosition().y + downBtn_.getSize().y / 2.f - 2.f });
}

void SpinBox::setValue(int v)
{
    value_ = std::max(minV_, std::min(maxV_, v));
    valueText_.setString(std::to_string(value_));
    Theme::centerOrigin(valueText_);
}

void SpinBox::updateHover(sf::Vector2f mousePos)
{
    bool overUp = upBtn_.getGlobalBounds().contains(mousePos);
    bool overDown = downBtn_.getGlobalBounds().contains(mousePos);
    upBtn_.setFillColor(overUp ? sf::Color(70, 58, 40) : sf::Color(40, 34, 30));
    downBtn_.setFillColor(overDown ? sf::Color(70, 58, 40) : sf::Color(40, 34, 30));
}

bool SpinBox::click(sf::Vector2f mousePos)
{
    if (upBtn_.getGlobalBounds().contains(mousePos)) { setValue(value_ + 1); return true; }
    if (downBtn_.getGlobalBounds().contains(mousePos)) { setValue(value_ - 1); return true; }
    return false;
}

void SpinBox::draw(sf::RenderTarget& target) const
{
    target.draw(box_);
    target.draw(upBtn_);
    target.draw(downBtn_);
    target.draw(upText_);
    target.draw(downText_);
    target.draw(valueText_);
}

// ── Chip ───────────────────────────────────────────────────────────────
Chip::Chip(const std::string& text, sf::Vector2f position, sf::Color color)
    : box_(sf::Vector2f(10.f, 10.f)), text_(Theme::bodyFont(), text, 13)
{
    text_.setFillColor(sf::Color::Black);
    sf::FloatRect b = text_.getLocalBounds();
    box_.setSize({ b.size.x + 16.f, b.size.y + 12.f });
    box_.setFillColor(color);
    setPosition(position);
}

void Chip::setPosition(sf::Vector2f pos)
{
    box_.setPosition(pos);
    text_.setPosition({ pos.x + 8.f, pos.y + 5.f });
}

void Chip::draw(sf::RenderTarget& target) const
{
    target.draw(box_);
    target.draw(text_);
}

sf::FloatRect Chip::getBounds() const { return box_.getGlobalBounds(); }