#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "core/Bord.hpp"
#include "graphics/CharacterView.hpp"

// ─────────────────────────────────────────────────────────────────────────
//  BoardView
//  Draws the fixed 32-space graph exactly as it is wired in Bord.cpp
//  (adjacency read straight from Bord::getposAdjacent, so the picture can
//  never drift out of sync with the real rules), plus whichever characters
//  currently occupy a space. GameWindow tells it which spaces are legal to
//  click right now (parsed from the engine's own "Available space(s)"
//  narration) so it can glow them; clicking anywhere still simply reports
//  the space index and lets the real Controller validate it.
// ─────────────────────────────────────────────────────────────────────────
class BoardView
{
public:
    BoardView(sf::Vector2f position, sf::Vector2f size);

    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);

    void setBoard(Bord* board); // read-only access, refreshed by GameWindow

    void setHighlighted(const std::vector<int>& spaces); // legal destinations
    void setSelected(int space);                          // -1 = none

    void updateHover(sf::Vector2f mousePos);
    int click(sf::Vector2f mousePos) const; // -1 if the click missed every node
    int hoveredSpace() const { return hoveredNode_; }

    void draw(sf::RenderTarget& target) const;

    // Normalized (0..1) layout position of a given space index (0-31).
    static sf::Vector2f nodeLayoutFraction(int index);

private:
    sf::Vector2f position_, size_;
    Bord* board_ = nullptr;
    std::vector<int> highlighted_;
    int selected_ = -1;
    int hoveredNode_ = -1;
    float nodeRadius_ = 16.f;

    sf::Vector2f nodePixelPos(int index) const;
    bool isSecretPassage(int index) const;
};