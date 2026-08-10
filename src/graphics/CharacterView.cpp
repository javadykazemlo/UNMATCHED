#include "graphics/CharacterView.hpp"
#include "graphics/UI.hpp"

CharacterView::CharacterView(sf::Vector2f position, float portraitRadius)
    : position_(position), radius_(portraitRadius)
{
}

void CharacterView::setCharacter(Character* character) { character_ = character; }
void CharacterView::setPosition(sf::Vector2f pos) { position_ = pos; }
void CharacterView::setRadius(float radius) { radius_ = radius; }
void CharacterView::setCompact(bool compact) { compact_ = compact; }
void CharacterView::setSelectable(bool selectable) { selectable_ = selectable; }
void CharacterView::setSelected(bool selected) { selected_ = selected; }
void CharacterView::setDimmed(bool dimmed) { dimmed_ = dimmed; }
void CharacterView::setDisplayIndex(int index) { displayIndex_ = index; }

sf::FloatRect CharacterView::getBounds() const
{
    float h = compact_ ? radius_ * 2.f : radius_ * 2.f + 40.f;
    return sf::FloatRect({ position_.x - radius_, position_.y - radius_ },
                          { radius_ * 2.f, h });
}

bool CharacterView::contains(sf::Vector2f point) const
{
    sf::Vector2f d = point - position_;
    return (d.x * d.x + d.y * d.y) <= radius_ * radius_;
}

void CharacterView::updateHover(sf::Vector2f mousePos)
{
    hovered_ = selectable_ && contains(mousePos);
}

void CharacterView::draw(sf::RenderTarget& target) const
{
    if (!character_) return;

    bool alive = character_->checkalive();
    sf::Color ownerColor = character_->getowner() == 1 ? Theme::Player1 : Theme::Player2;
    if (dimmed_ || !alive) ownerColor = sf::Color(70, 65, 70);

    sf::CircleShape circle(radius_);
    circle.setOrigin({ radius_, radius_ });
    circle.setPosition(position_);
    circle.setFillColor(sf::Color(20, 16, 22));
    circle.setOutlineThickness(selected_ ? 4.f : (hovered_ ? 3.f : 2.f));
    circle.setOutlineColor(selected_ ? Theme::GoldBright : ownerColor);
    target.draw(circle);

    // inner tint ring showing owner color -------------------------------------
    sf::CircleShape inner(radius_ - 4.f);
    inner.setOrigin({ radius_ - 4.f, radius_ - 4.f });
    inner.setPosition(position_);
    sf::Color fill = ownerColor;
    fill.a = 90;
    inner.setFillColor(fill);
    target.draw(inner);

    // initial letter -------------------------------------------------------
    std::string initial = character_->getName().empty() ? "?" : std::string(1, static_cast<char>(std::toupper(character_->getName()[0])));
    sf::Text letter(Theme::titleFont(), initial, static_cast<unsigned int>(radius_ * 1.1f));
    letter.setFillColor(alive ? Theme::TextLight : Theme::TextMuted);
    Theme::centerOrigin(letter);
    letter.setPosition(position_);
    target.draw(letter);

    // hero crown marker -------------------------------------------------------
    if (character_->isHero())
    {
        sf::Text crown(Theme::bodyFont(), "*", static_cast<unsigned int>(radius_ * 0.9f));
        crown.setFillColor(Theme::GoldBright);
        Theme::centerOrigin(crown);
        crown.setPosition({ position_.x + radius_ * 0.7f, position_.y - radius_ * 0.7f });
        target.draw(crown);
    }

    if (displayIndex_ >= 0)
    {
        sf::CircleShape badge(9.f);
        badge.setPosition({ position_.x - radius_ - 4.f, position_.y - radius_ - 4.f });
        badge.setFillColor(sf::Color(0, 0, 0, 200));
        badge.setOutlineColor(Theme::Gold);
        badge.setOutlineThickness(1.f);
        target.draw(badge);

        sf::Text idx(Theme::bodyFont(), std::to_string(displayIndex_), 12);
        idx.setFillColor(Theme::GoldBright);
        Theme::centerOrigin(idx);
        idx.setPosition({ position_.x - radius_ + 5.f, position_.y - radius_ + 5.f });
        target.draw(idx);
    }

    if (compact_) return;

    // name --------------------------------------------------------------------
    sf::Text name(Theme::bodyFont(), character_->getName(), 13);
    name.setFillColor(alive ? Theme::TextLight : Theme::TextMuted);
    Theme::centerOrigin(name);
    name.setPosition({ position_.x, position_.y + radius_ + 12.f });
    target.draw(name);

    // HP bar --------------------------------------------------------------------
    float barW = radius_ * 1.8f;
    sf::Vector2f barPos = { position_.x - barW / 2.f, position_.y + radius_ + 24.f };
    sf::RectangleShape back(sf::Vector2f(barW, 10.f));
    back.setPosition(barPos);
    back.setFillColor(sf::Color(35, 20, 22));
    back.setOutlineColor(Theme::PanelBorder);
    back.setOutlineThickness(1.f);
    target.draw(back);

    int hp = character_->getHp();
    int maxHp = character_->getMaxhp();
    float ratio = maxHp > 0 ? std::max(0.f, std::min(1.f, float(hp) / float(maxHp))) : 0.f;
    sf::RectangleShape hpFill(sf::Vector2f(barW * ratio, 10.f));
    hpFill.setPosition(barPos);
    hpFill.setFillColor(ratio > 0.5f ? Theme::Success : (ratio > 0.25f ? sf::Color(200, 160, 40) : Theme::Danger));
    target.draw(hpFill);

    sf::Text hpText(Theme::bodyFont(), std::to_string(hp) + "/" + std::to_string(maxHp), 11);
    hpText.setFillColor(Theme::TextLight);
    Theme::centerOrigin(hpText);
    hpText.setPosition({ position_.x, barPos.y + 5.f });
    target.draw(hpText);
}