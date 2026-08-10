#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "entities/Character.hpp"

// ─────────────────────────────────────────────────────────────────────────
//  CharacterView
//  Draws one Character as a circular portrait (initial letter, tinted by
//  owner) with name + HP bar underneath. Used both in the side panels
//  (full size) and directly on the board (compact size). Read-only view
//  over a Character*; never mutates it.
// ─────────────────────────────────────────────────────────────────────────
class CharacterView
{
public:
    explicit CharacterView(sf::Vector2f position = {0.f, 0.f}, float portraitRadius = 34.f);

    void setCharacter(Character* character);
    Character* getCharacter() const { return character_; }

    void setPosition(sf::Vector2f pos);
    void setRadius(float radius);
    void setCompact(bool compact);       // true => small board token, no name/HP text
    void setSelectable(bool selectable);
    void setSelected(bool selected);
    void setDimmed(bool dimmed);         // e.g. dead / not this player's turn
    void setDisplayIndex(int index);     // 1-based index used in Controller's prompts, -1 = hidden

    sf::FloatRect getBounds() const;
    bool contains(sf::Vector2f point) const;
    void updateHover(sf::Vector2f mousePos);

    void draw(sf::RenderTarget& target) const;

private:
    Character* character_ = nullptr;
    sf::Vector2f position_;
    float radius_;
    bool compact_ = false;
    bool selectable_ = false;
    bool selected_ = false;
    bool dimmed_ = false;
    bool hovered_ = false;
    int displayIndex_ = -1;
};