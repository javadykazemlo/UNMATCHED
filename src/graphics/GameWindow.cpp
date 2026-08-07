#include "graphics/GameWindow.hpp"

GameWindow::GameWindow(unsigned int width, unsigned int height, const std::string& title)
    : window(sf::VideoMode({width, height}), title)
{
    window.setFramerateLimit(60);
}

bool GameWindow::isOpen() const
{
    return window.isOpen();
}

void GameWindow::pollAndHandleEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
    }
}

void GameWindow::clear()
{
    window.clear(sf::Color(30, 30, 35));
}

void GameWindow::display()
{
    window.display();
}

sf::RenderWindow& GameWindow::getWindow()
{
    return window;
}