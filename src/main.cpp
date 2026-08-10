#include "graphics/GameWindow.hpp"
#include "core/Controller.hpp"
#include "core/Player.hpp"
#include "core/UIBridge.hpp"
#include <thread>
#include <atomic>

int main()
{
    Player players[2];
    Controller controller;
    UIBridge bridge;

    // From now on, getInt/getChoice/getYesNo/getLine inside Controller
    // will block waiting for bridge.answerXXX(...) instead of reading cin.
    // NOTE: no scene answers the bridge yet (that's the next step), so the
    // game will just sit at the first prompt without freezing the window.
    controller.setUIBridge(&bridge);

    std::atomic<bool> logicFinished{false};

    std::thread logicThread([&]()
    {
        controller.startMenu(players);
        controller.playTurn();
        logicFinished = true;
    });

    GameWindow gw;
    while (gw.isOpen())
    {
        gw.pollAndHandleEvents();
        gw.clear();
        // gw.renderBoard(controller.getBord().snapshot());
        gw.display();
    }

    if (logicThread.joinable())
        logicThread.join();

    return 0;
}