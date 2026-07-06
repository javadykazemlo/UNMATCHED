#pragma once
#include "entities/Character.hpp"
#include "core/Bord.hpp"
#include "core/Player.hpp"

class Dracula : public Character
{
private:

    Player* currentPlayer;
    Bord* gameBoard;
public:
    Dracula();
    void setCurrentPlayer(Player* player);
    void setGameBoard(Bord* board);

    void ability() override;

    ~Dracula();
};