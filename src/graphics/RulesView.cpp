#include "graphics/RulesView.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace
{
    const sf::Color GOLD(211, 178, 104);
    const sf::Color PALE_GOLD(236, 218, 174);
    const sf::Color PARCHMENT(226, 216, 190);
    const sf::Color MUTED(166, 157, 143);
    const sf::Color RED(145, 35, 39);
    const sf::Color BLUE(40, 82, 123);
    const sf::Color PANEL(9, 10, 16, 250);
    const sf::Color PANEL_2(17, 17, 25, 245);
}

RulesView::RulesView(const sf::Font& font_) : font(font_)
{
    buildSections();
}

void RulesView::open()
{
    opened = true;
    selectedSection = 0;
    scroll = 0.f;
}

void RulesView::close()
{
    opened = false;
}

bool RulesView::isOpen() const
{
    return opened;
}
 
void RulesView::buildSections()
{
    sections = {
        {"OBJECTIVE", {
            "The goal of Unmatched is to defeat your opponent's Hero. A Hero is defeated when their Health reaches 0.",
            "Defeating a Sidekick alone does not end the game. If your opponent's Hero is defeated, the game immediately ends and you win."
        }},
        {"GAME SETUP", {
            "Choose a battlefield and a Hero. Take that Hero's action deck and character components, then set the Hero and Sidekick Health shown by the character cards.",
            "Shuffle your action deck and draw 5 cards. Place the Heroes and Sidekicks in their legal starting spaces. Determine the first player, then begin the first turn.",
            "This Rules screen describes the two-player setup supported by this project; it does not claim to implement every official Unmatched setup variation."
        }},
        {"THE BATTLEFIELD", {
            "The battlefield is made of numbered spaces connected by lines. Connected spaces are adjacent. A space normally holds only one fighter.",
            "Zones are groups of spaces marked by colors. A space can belong to more than one Zone. Adjacency is important for movement and Melee attacks; Zones are important for Ranged attacks and card effects.",
            "In this project the board also displays its logical space and Zone information so the rules can be checked directly against the game board."
        }},
        {"YOUR TURN", {
            "On your turn you take exactly 2 actions. You may choose Maneuver, Attack, or Scheme, and you may choose the same type of action twice.",
            "The on-screen controls such as MOVE, ATTACK and SCHEME are shortcuts for the corresponding game actions. DRAW CARD and BOOST are not additional standard actions; they are handled as parts of the project's action flow."
        }},
        {"MANEUVER", {
            "When you take a Maneuver action, draw 1 card from your deck, then you may move your fighters.",
            "Each fighter may move up to its movement value through adjacent spaces. Friendly fighters may be moved through, but a fighter cannot normally move through an opposing fighter and cannot end movement in an occupied space.",
            "You may move a fighter fewer spaces, including 0 spaces. Movement must follow the actual adjacency graph of the battlefield."
        }},
        {"ATTACK", {
            "To attack, choose an attacking fighter, a legal opposing target, and an Attack or Versatile card from your hand. The defender may then choose a legal Defense or Versatile card.",
            "Reveal the cards, resolve their relevant effects and timing, compare the attack and defense values, apply combat damage, then resolve After Combat effects and discard the cards that were played."
        }},
        {"SCHEME", {
            "Scheme cards are played during a Scheme action. The player declares the eligible fighter using the Scheme, then resolves the card's effect.",
            "After the effect resolves, the Scheme normally goes to the discard pile unless its own text says otherwise. Scheme cards are not Attack or Defense cards."
        }},
        {"BOOST", {
            "During a Maneuver, you may Boost movement by discarding a card from your hand. Add that card's Boost value to the fighter's movement value for that Maneuver.",
            "The discarded card is used only for its Boost value; its normal card effect is not resolved.",
            "Example: Movement 2 & Boost 3 : up to 5 spaces of movement for that fighter."
        }},
        {"COMBAT", {
            "Combat compares the attack value with the defense value after the appropriate card effects have resolved. If the attack exceeds the defense, the difference is combat damage.",
            "Card effects may occur before, during, or after combat. Timing matters. When simultaneous effects need an order, the defender's effect resolves first.",
            "Example: Attack 5 against Defense 3 results in 2 combat damage before any additional applicable effects."
        }},
        {"FIGHTERS", {
            "Each Hero and Sidekick is a Fighter. Fighters have Health and may have their own movement, attack range, and special abilities.",
            "When a fighter reaches 0 Health, it is defeated. A defeated Sidekick is removed from the battlefield. A defeated Hero causes that player's immediate loss."
        }},
        {"CARDS & DECK", {
            "Each Hero has an action deck. The normal starting hand is 5 cards. Cards played during the game normally move to the face-up discard pile, which players may inspect.",
            "The normal hand limit is 7 cards. If you have more than 7 cards at the end of your turn, discard down to 7.",
            "When your deck is empty, you become exhausted. If you would need to draw a card while exhausted, instead each of your fighters takes 2 damage. The discard pile is not automatically reshuffled unless a specific rule or card says so.",
            "Versatile cards can be used as either Attack or Defense and count as both types for effects that check card type."
        }},
        {"ZONES & ATTACK RANGE", {
            "Melee fighters can attack an adjacent opposing fighter. Ranged fighters can attack an adjacent opposing fighter or an opposing fighter anywhere in the same Zone.",
            "A shared-color space belongs to multiple Zones. That means fighters in compatible shared Zones can satisfy Ranged attack requirements even when they are not adjacent.",
            "Ranged attacks are not unrestricted attacks across the whole board; Zone membership determines the additional reach."
        }},
        {"WINNING THE GAME", {
            "The game is won when the opposing Hero is defeated. Sidekick defeats do not by themselves win the game.",
            "A combat is considered won by the attacker when the attack itself deals at least 1 combat damage. The defender wins the combat when the attack deals no combat damage.",
            "Damage caused only by a separate card effect is not combat damage and does not by itself change who won the combat."
        }},
        {"SPECIAL RULES", {
            "Secret passages in this project connect specific board spaces and can be used by the movement implementation. They should not automatically be treated as ordinary adjacency for Melee attacks or unrelated effects.",
            "Special card text and character abilities can override normal rules. Always resolve the specific card or character instruction when it conflicts with a general rule.",
            "The project's implementation is the source of truth for controls and map-specific behavior. This Rules screen does not alter the underlying game engine."
        }},
        {"HOW TO PLAY THIS VERSION", {
            "MOVE : activates movement selection. Choose one of your fighters and then choose a highlighted legal destination.",
            "ATTACK : activates attack selection. Choose an appropriate Attack/Versatile card and an enemy target that the current fighter can legally attack.",
            "SCHEME : enters the Scheme flow. Scheme cards are selected from your hand and resolved through the existing game logic.",
            "BOOST : selects a card to use for its Boost value during movement. The current implementation handles this through the existing action flow.",
            "DRAW CARD : draws one card from the current deck and ends the current action through the existing Controller rule path.",
            "PLAY CARD  plays the currently selected card through the existing Deck/Controller implementation.",
            "END ACTION  finishes the current action. END TURN finishes the turn and passes control to the next player.",
            "While this Rules window is open, all game interaction behind it is disabled. Press ESC or CLOSE to return to the exact game state you left."
        }}
    };
}

sf::FloatRect RulesView::panelRect(sf::Vector2f viewportSize) const
{
    const float w = std::min(panelWidth, viewportSize.x - 40.f);
    const float h = std::min(panelHeight, viewportSize.y - 40.f);
    return {{(viewportSize.x - w) / 2.f, (viewportSize.y - h) / 2.f}, {w, h}};
}

sf::FloatRect RulesView::contentRect(sf::Vector2f viewportSize) const
{
    const sf::FloatRect panel = panelRect(viewportSize);
    return {{panel.position.x + navigationWidth + 18.f,
              panel.position.y + headerHeight + 12.f},
            {panel.size.x - navigationWidth - 38.f,
             panel.size.y - headerHeight - footerHeight - 24.f}};
}

sf::FloatRect RulesView::closeRect(sf::Vector2f viewportSize) const
{
    const sf::FloatRect panel = panelRect(viewportSize);
    return {{panel.position.x + panel.size.x - 128.f,
              panel.position.y + panel.size.y - 43.f},
            {108.f, 28.f}};
}

void RulesView::clampScroll(float contentHeight, float viewportHeight)
{
    const float maxScroll = std::max(0.f, contentHeight - viewportHeight);
    scroll = std::clamp(scroll, 0.f, maxScroll);
}

bool RulesView::handleEvent(const sf::Event& event, sf::Vector2f mousePosition)
{
    if (!opened) return false;

    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape)
        {
            close();
            return true;
        }
    }

    if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        const sf::Vector2f viewport(1600.f, 900.f);
        const sf::FloatRect content = contentRect(viewport);
        if (content.contains(mousePosition))
        {
            scroll -= wheel->delta * 42.f;
            scroll = std::max(0.f, scroll);
        }
        return true;
    }

    if (const auto* mouse = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouse->button != sf::Mouse::Button::Left) return true;

        const sf::Vector2f viewport(1600.f, 900.f);
        const sf::FloatRect panel = panelRect(viewport);
        const sf::FloatRect closeButton = closeRect(viewport);
        if (closeButton.contains(mousePosition))
        {
            close();
            return true;
        }

        const sf::FloatRect nav({panel.position.x + 14.f, panel.position.y + headerHeight + 12.f},
                                {navigationWidth - 28.f, panel.size.y - headerHeight - footerHeight - 24.f});
        if (nav.contains(mousePosition))
        {
            const float itemHeight = 38.f;
            const int index = static_cast<int>((mousePosition.y - nav.position.y) / itemHeight);
            if (index >= 0 && index < static_cast<int>(sections.size()))
            {
                selectedSection = static_cast<std::size_t>(index);
                scroll = 0.f;
            }
        }
        return true;
    }

    return true;
}

void RulesView::drawText(sf::RenderTarget& target, const std::string& text,
                         sf::Vector2f position, unsigned size, sf::Color color) const
{
    sf::Text t(font, text, size);
    t.setPosition(position);
    t.setFillColor(color);
    target.draw(t);
}

float RulesView::drawWrappedText(sf::RenderTarget& target, const std::string& text,
                                sf::FloatRect area, unsigned size, sf::Color color,
                                float lineSpacing) const
{
    std::istringstream words(text);
    std::string word;
    std::string line;
    float y = area.position.y;
    const float lineHeight = static_cast<float>(size) * lineSpacing;

    auto drawLine = [&](const std::string& value)
    {
        if (value.empty())
        {
            y += lineHeight;
            return;
        }
        sf::Text t(font, value, size);
        t.setPosition({area.position.x, y});
        t.setFillColor(color);
        target.draw(t);
        y += lineHeight;
    };

    while (words >> word)
    {
        const std::string candidate = line.empty() ? word : line + " " + word;
        sf::Text measure(font, candidate, size);
        if (!line.empty() && measure.getLocalBounds().size.x > area.size.x)
        {
            drawLine(line);
            line = word;
        }
        else
        {
            line = candidate;
        }
    }
    drawLine(line);
    return y;
}

void RulesView::draw(sf::RenderTarget& target, sf::Vector2f viewportSize) const
{
    if (!opened) return;

    const sf::FloatRect panel = panelRect(viewportSize);

    sf::RectangleShape darkener(viewportSize);
    darkener.setFillColor(sf::Color(0, 0, 0, 175));
    target.draw(darkener);

    sf::RectangleShape outer(panel.size);
    outer.setPosition(panel.position);
    outer.setFillColor(PANEL);
    outer.setOutlineColor(GOLD);
    outer.setOutlineThickness(2.f);
    target.draw(outer);

    sf::RectangleShape inner({panel.size.x - 10.f, panel.size.y - 10.f});
    inner.setPosition({panel.position.x + 5.f, panel.position.y + 5.f});
    inner.setFillColor(PANEL_2);
    inner.setOutlineColor(sf::Color(GOLD.r, GOLD.g, GOLD.b, 70));
    inner.setOutlineThickness(1.f);
    target.draw(inner);

    // Header.
    drawText(target, "UNMATCHED", {panel.position.x + 22.f, panel.position.y + 11.f}, 25, GOLD);
    drawText(target, "HOW TO PLAY", {panel.position.x + 24.f, panel.position.y + 40.f}, 10, MUTED);

    sf::RectangleShape headerLine({panel.size.x - 40.f, 1.f});
    headerLine.setPosition({panel.position.x + 20.f, panel.position.y + headerHeight - 1.f});
    headerLine.setFillColor(sf::Color(GOLD.r, GOLD.g, GOLD.b, 100));
    target.draw(headerLine);

    // Navigation.
    const sf::FloatRect nav({panel.position.x + 14.f, panel.position.y + headerHeight + 12.f},
                            {navigationWidth - 28.f, panel.size.y - headerHeight - footerHeight - 24.f});
    sf::RectangleShape navPanel(nav.size);
    navPanel.setPosition(nav.position);
    navPanel.setFillColor(sf::Color(7, 8, 13, 210));
    navPanel.setOutlineColor(sf::Color(82, 72, 55));
    navPanel.setOutlineThickness(1.f);
    target.draw(navPanel);

    const float itemHeight = std::min(38.f, (nav.size.y - 10.f) / static_cast<float>(sections.size()));
    for (std::size_t i = 0; i < sections.size(); ++i)
    {
        const sf::FloatRect item({nav.position.x + 5.f, nav.position.y + 5.f + itemHeight * static_cast<float>(i)},
                                 {nav.size.x - 10.f, itemHeight - 3.f});
        if (i == selectedSection)
        {
            sf::RectangleShape selected(item.size);
            selected.setPosition(item.position);
            selected.setFillColor(sf::Color(74, 16, 21, 220));
            selected.setOutlineColor(RED);
            selected.setOutlineThickness(1.f);
            target.draw(selected);
        }

        std::string number = std::to_string(i + 1) + ".";
        drawText(target, number, {item.position.x + 9.f, item.position.y + 8.f}, 9,
                 i == selectedSection ? GOLD : MUTED);
        drawText(target, sections[i].title, {item.position.x + 30.f, item.position.y + 6.f}, 10,
                 i == selectedSection ? PALE_GOLD : PARCHMENT);
    }

    // Content area.
    const sf::FloatRect content = contentRect(viewportSize);
    sf::RectangleShape contentPanel(content.size);
    contentPanel.setPosition(content.position);
    contentPanel.setFillColor(sf::Color(12, 13, 20, 215));
    contentPanel.setOutlineColor(sf::Color(82, 72, 55));
    contentPanel.setOutlineThickness(1.f);
    target.draw(contentPanel);

    const Section& section = sections[selectedSection];
    const float innerX = content.position.x + 25.f;
    const float innerW = content.size.x - 50.f;

    float contentHeight = 75.f;
    for (const std::string& paragraph : section.paragraphs)
    {
        std::istringstream words(paragraph);
        std::string word, line;
        int lines = 0;
        while (words >> word)
        {
            const std::string candidate = line.empty() ? word : line + " " + word;
            sf::Text measure(font, candidate, 13);
            if (!line.empty() && measure.getLocalBounds().size.x > innerW)
            {
                ++lines;
                line = word;
            }
            else
            {
                line = candidate;
            }
        }
        if (!line.empty()) ++lines;
        contentHeight += lines * 13.f * 1.42f + 14.f;
    }

    const float maxScroll = std::max(0.f, contentHeight - content.size.y);
    const float effectiveScroll = std::clamp(scroll, 0.f, maxScroll);
    float y = content.position.y + 22.f - effectiveScroll;

    sf::Text title(font, section.title, 20);
    title.setPosition({innerX, y});
    title.setFillColor(GOLD);
    target.draw(title);
    y += 34.f;

    sf::RectangleShape divider({innerW, 1.f});
    divider.setPosition({innerX, y});
    divider.setFillColor(sf::Color(GOLD.r, GOLD.g, GOLD.b, 75));
    target.draw(divider);
    y += 20.f;

    for (const std::string& paragraph : section.paragraphs)
    {
        const bool bullet = paragraph.rfind("-", 0) == 0 || paragraph.rfind("-", 0) == 0;
        const std::string body = bullet ? "• " + paragraph.substr(1) : paragraph;
        y = drawWrappedText(target, body, {{innerX, y}, {innerW, 1800.f}}, 13, PARCHMENT, 1.42f);
        y += 14.f;
    }

    if (section.title == "THE BATTLEFIELD" || section.title == "ZONES & ATTACK RANGE")
    {
        const float dy = y;
        if (dy > content.position.y - scroll && dy < content.position.y + content.size.y + 80.f)
        {
            sf::CircleShape a(20.f), b(20.f), c(20.f);
            a.setOrigin({20.f,20.f}); b.setOrigin({20.f,20.f}); c.setOrigin({20.f,20.f});
            a.setPosition({innerX + 55.f, dy + 30.f});
            b.setPosition({innerX + 155.f, dy + 30.f});
            c.setPosition({innerX + 255.f, dy + 30.f});
            a.setFillColor(sf::Color(45, 69, 86));
            b.setFillColor(sf::Color(65, 80, 57));
            c.setFillColor(sf::Color(45, 69, 86));
            a.setOutlineColor(GOLD); b.setOutlineColor(GOLD); c.setOutlineColor(GOLD);
            a.setOutlineThickness(1.f); b.setOutlineThickness(1.f); c.setOutlineThickness(1.f);
            target.draw(a); target.draw(b); target.draw(c);
            drawText(target, "1", {innerX + 51.f, dy + 20.f}, 10, PARCHMENT);
            drawText(target, "2", {innerX + 151.f, dy + 20.f}, 10, PARCHMENT);
            drawText(target, "3", {innerX + 251.f, dy + 20.f}, 10, PARCHMENT);

            sf::RectangleShape line1({60.f, 2.f});
            line1.setPosition({innerX + 75.f, dy + 29.f});
            line1.setFillColor(GOLD);
            target.draw(line1);
            sf::RectangleShape line2({60.f, 2.f});
            line2.setPosition({innerX + 175.f, dy + 29.f});
            line2.setFillColor(GOLD);
            target.draw(line2);
            drawText(target, "Adjacent spaces", {innerX + 330.f, dy + 20.f}, 10, MUTED);
        }
    }

    // Footer.
    const float footerY = panel.position.y + panel.size.y - footerHeight;
    sf::RectangleShape footerLine({panel.size.x - 40.f, 1.f});
    footerLine.setPosition({panel.position.x + 20.f, footerY});
    footerLine.setFillColor(sf::Color(GOLD.r, GOLD.g, GOLD.b, 70));
    target.draw(footerLine);

    sf::RectangleShape closeButton(closeRect(viewportSize).size);
    closeButton.setPosition(closeRect(viewportSize).position);
    closeButton.setFillColor(sf::Color(58, 13, 18, 245));
    closeButton.setOutlineColor(GOLD);
    closeButton.setOutlineThickness(1.5f);
    target.draw(closeButton);
    drawText(target, "CLOSE", {closeButton.getPosition().x + 33.f,
                                closeButton.getPosition().y + 6.f}, 10, PALE_GOLD);
    drawText(target, "ESC  CLOSE", {panel.position.x + 24.f, footerY + 10.f}, 10, MUTED);
    drawText(target, "Mouse wheel  SCROLL", {panel.position.x + 150.f, footerY + 10.f}, 10, MUTED);

    // Scrollbar.
    if (maxScroll > 0.f)
    {
        const float trackX = content.position.x + content.size.x - 10.f;
        const float trackY = content.position.y + 10.f;
        const float trackH = content.size.y - 20.f;
        sf::RectangleShape track({4.f, trackH});
        track.setPosition({trackX, trackY});
        track.setFillColor(sf::Color(45, 43, 40, 220));
        target.draw(track);

        const float thumbH = std::max(35.f, trackH * (content.size.y / contentHeight));
        const float thumbY = trackY + (trackH - thumbH) * (effectiveScroll / maxScroll);
        sf::RectangleShape thumb({4.f, thumbH});
        thumb.setPosition({trackX, thumbY});
        thumb.setFillColor(GOLD);
        target.draw(thumb);
    }
}
