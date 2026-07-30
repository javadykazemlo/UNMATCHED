#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "core/Bord.hpp"
#include "core/Player.hpp"
#include "cards/Card.hpp"
#include "cards/Deck.hpp"
#include "entities/Character.hpp"


class SaveManager
{
public:
    static bool saveGame(Player* current, Player* enemy,int gamerand, bool cancelEffectDR, bool cancelEffectSH,bool cancelEffectIM, bool guessElementary,const std::string& filepath = "save.json");

    static bool loadGame(Bord& bord, Player players[2],Player*& currentOut, Player*& enemyOut,int& gamerandOut, bool& cancelEffectDROut, bool& cancelEffectSHOut,bool& cancelEffectIMOut, bool& guessElementaryOut,const std::string& filepath = "save.json");

private:
    static nlohmann::json cardToJson(const Card& card);
    static Card cardFromJson(const nlohmann::json& j);

    static nlohmann::json characterToJson(Character* character);
    static nlohmann::json deckToJson(Deck* deck);

    static nlohmann::json playerToJson(Player& player);
    static void playerFromJson(Player& player, const nlohmann::json& j);
    static int heroChoiceFromName(const std::string& name);
};