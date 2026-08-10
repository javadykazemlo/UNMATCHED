#include "graphics/CardView.hpp"
#include "graphics/UI.hpp"

CardView::CardView(sf::Vector2f position, sf::Vector2f size)
    : position_(position), size_(size)
{
}

void CardView::setCard(const Card& card) { card_ = card; hasCard_ = true; }
void CardView::setPosition(sf::Vector2f pos) { position_ = pos; }
void CardView::setSize(sf::Vector2f size) { size_ = size; }
void CardView::setFaceDown(bool faceDown) { faceDown_ = faceDown; }
void CardView::setSelected(bool selected) { selected_ = selected; }
void CardView::setHighlighted(bool highlighted) { highlighted_ = highlighted; }
void CardView::setIndex(int handIndex) { index_ = handIndex; }

sf::FloatRect CardView::getBounds() const
{
    return sf::FloatRect(position_, size_);
}

bool CardView::contains(sf::Vector2f point) const
{
    return getBounds().contains(point);
}

void CardView::updateHover(sf::Vector2f mousePos)
{
    hovered_ = contains(mousePos);
}

sf::Color CardView::colorForType(CardType type)
{
    switch (type)
    {
        case CardType::Attack:     return Theme::AttackCard;
        case CardType::Defense:    return Theme::DefenseCard;
        case CardType::Versatile:  return Theme::VersatileCard;
        case CardType::Scheme:     return Theme::SchemeCard;
    }
    return Theme::PanelBorder;
}

void CardView::draw(sf::RenderTarget& target) const
{
    sf::Vector2f pos = position_;
    if (hovered_ && !faceDown_) pos.y -= 10.f; // lift on hover, like a hand of cards

    sf::RectangleShape body(size_);
    body.setPosition(pos);
    body.setFillColor(sf::Color(24, 20, 26));
    body.setOutlineThickness(selected_ ? 3.f : 2.f);
    body.setOutlineColor(selected_ ? Theme::GoldBright
                          : highlighted_ ? Theme::Success
                          : Theme::PanelBorder);
    target.draw(body);

    if (faceDown_ || !hasCard_)
    {
        sf::RectangleShape inner(sf::Vector2f(size_.x - 14.f, size_.y - 14.f));
        inner.setPosition({ pos.x + 7.f, pos.y + 7.f });
        inner.setFillColor(sf::Color(16, 13, 18));
        inner.setOutlineThickness(1.f);
        inner.setOutlineColor(Theme::Gold);
        target.draw(inner);

        sf::Text mark(Theme::titleFont(), "U", static_cast<unsigned int>(size_.x * 0.35f));
        mark.setFillColor(Theme::Gold);
        Theme::centerOrigin(mark);
        mark.setPosition({ pos.x + size_.x / 2.f, pos.y + size_.y / 2.f });
        target.draw(mark);
        return;
    }

    // header strip colored by card type -------------------------------------
    sf::Color typeColor = colorForType(card_.getType());
    sf::RectangleShape header(sf::Vector2f(size_.x, size_.y * 0.16f));
    header.setPosition(pos);
    header.setFillColor(typeColor);
    target.draw(header);

    // attack / value badge ----------------------------------------------------
    float badgeR = size_.x * 0.16f;
    sf::CircleShape badge(badgeR);
    badge.setOrigin({ badgeR, badgeR });
    badge.setPosition({ pos.x + size_.x - badgeR - 4.f, pos.y + badgeR + 4.f });
    badge.setFillColor(sf::Color(10, 8, 12));
    badge.setOutlineColor(Theme::Gold);
    badge.setOutlineThickness(2.f);
    target.draw(badge);

    sf::Text valueText(Theme::titleFont(), std::to_string(card_.getAttack()),
                        static_cast<unsigned int>(badgeR * 1.1f));
    valueText.setFillColor(Theme::GoldBright);
    Theme::centerOrigin(valueText);
    valueText.setPosition(badge.getPosition());
    target.draw(valueText);

    // name --------------------------------------------------------------------
    sf::Text name(Theme::titleFont(), card_.getName(), 15);
    name.setFillColor(Theme::TextLight);
    name.setStyle(sf::Text::Bold);
    name.setPosition({ pos.x + 8.f, pos.y + size_.y * 0.16f + 6.f });
    // shrink to fit width
    while (name.getLocalBounds().size.x > size_.x - badgeR * 2.f - 12.f && name.getCharacterSize() > 9)
    {
        name.setCharacterSize(name.getCharacterSize() - 1);
    }
    target.draw(name);

    // type / timing tag ---------------------------------------------------
    sf::Text tag(Theme::bodyFont(), card_.getTypeString() + " \u00B7 " + card_.getTimingString(), 10);
    tag.setFillColor(Theme::TextMuted);
    tag.setPosition({ pos.x + 8.f, pos.y + size_.y * 0.16f + 26.f });
    target.draw(tag);

    // effect text -----------------------------------------------------------
    sf::Text effect(Theme::bodyFont(), "", 11);
    effect.setFillColor(Theme::TextLight);
    effect.setString(Theme::wordWrap(Theme::bodyFont(), card_.geteffect(), 11, size_.x - 16.f));
    effect.setPosition({ pos.x + 8.f, pos.y + size_.y * 0.16f + 44.f });
    target.draw(effect);

    // boost badge, bottom-left ----------------------------------------------
    if (card_.getBoost() > 0)
    {
        sf::Text boost(Theme::bodyFont(), "+" + std::to_string(card_.getBoost()) + " boost", 11);
        boost.setFillColor(Theme::Gold);
        boost.setPosition({ pos.x + 8.f, pos.y + size_.y - 20.f });
        target.draw(boost);
    }

    // owner marker, bottom-right ---------------------------------------------
    sf::Text owner(Theme::bodyFont(), card_.getOwnerString(), 10);
    owner.setFillColor(Theme::TextMuted);
    sf::FloatRect ob = owner.getLocalBounds();
    owner.setPosition({ pos.x + size_.x - ob.size.x - 8.f, pos.y + size_.y - 20.f });
    target.draw(owner);

    // hand index chip, top-left ----------------------------------------------
    if (index_ >= 0)
    {
        sf::CircleShape idx(10.f);
        idx.setPosition({ pos.x + 4.f, pos.y + 4.f });
        idx.setFillColor(sf::Color(0, 0, 0, 180));
        idx.setOutlineColor(Theme::Gold);
        idx.setOutlineThickness(1.f);
        target.draw(idx);

        sf::Text idxText(Theme::bodyFont(), std::to_string(index_), 12);
        idxText.setFillColor(Theme::GoldBright);
        Theme::centerOrigin(idxText);
        idxText.setPosition({ pos.x + 14.f, pos.y + 14.f });
        target.draw(idxText);
    }
}