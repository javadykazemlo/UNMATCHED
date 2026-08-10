#include "graphics/DeckView.hpp"
#include "graphics/UI.hpp"
#include <algorithm>

DeckView::DeckView(sf::Vector2f position, sf::Vector2f size)
    : position_(position), size_(size)
{
}

void DeckView::setPosition(sf::Vector2f pos) { position_ = pos; layout(); }
void DeckView::setSize(sf::Vector2f size) { size_ = size; layout(); }
void DeckView::setDeckCount(int count) { deckCount_ = count; }
void DeckView::setDiscardCount(int count) { discardCount_ = count; }
void DeckView::setExpanded(bool expanded) { expanded_ = expanded; }

void DeckView::setPlayableIndices(const std::vector<int>& indices) { playable_ = indices; }
void DeckView::clearPlayableIndices() { playable_.clear(); }

void DeckView::setHand(const std::vector<Card>& hand)
{
    cardViews_.clear();
    cardViews_.reserve(hand.size());
    for (const Card& c : hand)
    {
        CardView view;
        view.setCard(c);
        cardViews_.push_back(view);
    }
    layout();
}

void DeckView::layout()
{
    if (cardViews_.empty()) return;

    float cardW = 100.f, cardH = 140.f;
    float usableW = size_.x - 20.f;
    float step = cardViews_.size() > 1
        ? std::min(cardW + 8.f, (usableW - cardW) / static_cast<float>(cardViews_.size() - 1))
        : 0.f;

    float totalW = cardW + step * (cardViews_.size() > 0 ? static_cast<float>(cardViews_.size() - 1) : 0.f);
    float startX = position_.x + (size_.x - totalW) / 2.f;
    float y = position_.y + size_.y - cardH - 10.f;

    for (std::size_t i = 0; i < cardViews_.size(); ++i)
    {
        cardViews_[i].setSize({ cardW, cardH });
        cardViews_[i].setPosition({ startX + step * static_cast<float>(i), y });
        cardViews_[i].setIndex(static_cast<int>(i) + 1);
        bool playable = playable_.empty() ||
            std::find(playable_.begin(), playable_.end(), static_cast<int>(i) + 1) != playable_.end();
        cardViews_[i].setHighlighted(playable && !playable_.empty());
    }
}

void DeckView::updateHover(sf::Vector2f mousePos)
{
    for (auto& cv : cardViews_) cv.updateHover(mousePos);
}

int DeckView::click(sf::Vector2f mousePos)
{
    // iterate back-to-front since later cards are drawn on top / overlap
    for (int i = static_cast<int>(cardViews_.size()) - 1; i >= 0; --i)
    {
        if (cardViews_[i].contains(mousePos))
            return i + 1; // 1-based, matching Controller's hand indices
    }
    return -1;
}

void DeckView::draw(sf::RenderTarget& target) const
{
    // deck (draw pile) chip, left side ------------------------------------
    sf::RectangleShape deckPile(sf::Vector2f(70.f, 96.f));
    deckPile.setPosition({ position_.x + 10.f, position_.y + size_.y - 106.f });
    deckPile.setFillColor(sf::Color(20, 16, 22));
    deckPile.setOutlineColor(Theme::PanelBorder);
    deckPile.setOutlineThickness(2.f);
    target.draw(deckPile);

    sf::Text deckLabel(Theme::bodyFont(), "DECK\n" + std::to_string(deckCount_), 14);
    deckLabel.setFillColor(Theme::TextLight);
    Theme::centerOrigin(deckLabel);
    deckLabel.setPosition({ deckPile.getPosition().x + 35.f, deckPile.getPosition().y + 48.f });
    target.draw(deckLabel);

    // discard pile chip, right side ----------------------------------------
    sf::RectangleShape discardPile(sf::Vector2f(70.f, 96.f));
    discardPile.setPosition({ position_.x + size_.x - 80.f, position_.y + size_.y - 106.f });
    discardPile.setFillColor(sf::Color(20, 16, 22));
    discardPile.setOutlineColor(Theme::PanelBorder);
    discardPile.setOutlineThickness(2.f);
    target.draw(discardPile);

    sf::Text discardLabel(Theme::bodyFont(), "DISCARD\n" + std::to_string(discardCount_), 13);
    discardLabel.setFillColor(Theme::TextLight);
    Theme::centerOrigin(discardLabel);
    discardLabel.setPosition({ discardPile.getPosition().x + 35.f, discardPile.getPosition().y + 48.f });
    target.draw(discardLabel);

    // hand -----------------------------------------------------------------
    sf::Text title(Theme::bodyFont(), "YOUR HAND \u00B7 " + std::to_string(cardViews_.size()) + " CARDS", 13);
    title.setFillColor(Theme::Gold);
    title.setPosition({ position_.x + 100.f, position_.y + 4.f });
    target.draw(title);

    if (expanded_)
    {
        for (const auto& cv : cardViews_) cv.draw(target);
    }
    else
    {
        sf::Text tap(Theme::bodyFont(), "TAP TO VIEW HAND", 14);
        tap.setFillColor(Theme::TextMuted);
        Theme::centerOrigin(tap);
        tap.setPosition({ position_.x + size_.x / 2.f, position_.y + size_.y - 60.f });
        target.draw(tap);
    }
}