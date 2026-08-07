#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class GameWindow
{
private:
    sf::RenderWindow window;

public:
    GameWindow(unsigned int width = 1280, unsigned int height = 800,
               const std::string& title = "Unmatched");

    bool isOpen() const;
    void pollAndHandleEvents();
    void clear();
    void display();
    sf::RenderWindow& getWindow();

    ~GameWindow() = default;
};