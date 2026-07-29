#include <fstream>
#include <iostream>
#include <vector>

#include "Save/SaveManager.hpp"
#include "entities/invisible_man.hpp"

using json = nlohmann::json;
using namespace std;

// ---------------------------------------------------------------------
// Card <-> json
// ---------------------------------------------------------------------
json SaveManager::cardToJson(const Card& card)
{
    json j;
    j["name"]   = card.getName();
    j["owner"]  = static_cast<int>(card.getOwner());
    j["type"]   = static_cast<int>(card.getType());
    j["attack"] = card.getAttack();
    j["timing"] = static_cast<int>(card.getTiming());
    j["boost"]  = card.getBoost();
    j["effect"] = card.geteffect();
    return j;
}

Card SaveManager::cardFromJson(const json& j)
{
    return Card(
        j.at("name").get<string>(),
        static_cast<owner>(j.at("owner").get<int>()),
        static_cast<CardType>(j.at("type").get<int>()),
        j.at("attack").get<int>(),
        static_cast<timing>(j.at("timing").get<int>()),
        j.at("boost").get<int>(),
        j.at("effect").get<string>()
    );
}

// ---------------------------------------------------------------------
// Character -> json (loading characters happens through
// Player::chooseCharacter, see playerFromJson below)
// ---------------------------------------------------------------------
json SaveManager::characterToJson(Character* character)
{
    json j;
    j["name"]       = character->getName();
    j["hp"]         = character->getHp();
    j["maxHp"]      = character->getMaxhp();
    j["move"]       = character->getMove();
    j["attackType"] = character->getAttacktype();
    j["space"]      = character->getSpace();
    j["owner"]      = character->getowner();
    j["isHero"]     = static_cast<bool>(character->isHero());

    // Invisible Man carries extra state (his 3 fog tokens).
    if (invisible_man* im = dynamic_cast<invisible_man*>(character))
    {
        json tokens = json::array();
        for (int pos : im->getMistTokens())
            tokens.push_back(pos);
        j["mistTokens"] = tokens;
    }

    return j;
}

// ---------------------------------------------------------------------
// Deck -> json
// ---------------------------------------------------------------------
json SaveManager::deckToJson(Deck* deck)
{
    json j;

    json deckArr = json::array();
    for (const Card& c : deck->getdeck())
        deckArr.push_back(cardToJson(c));
    j["deckCards"] = deckArr;

    json handArr = json::array();
    for (const Card& c : deck->gethand())
        handArr.push_back(cardToJson(c));
    j["handCards"] = handArr;

    json discardArr = json::array();
    for (const Card& c : deck->getDiscardPile())
        discardArr.push_back(cardToJson(c));
    j["discardCards"] = discardArr;

    return j;
}

// ---------------------------------------------------------------------
// hero name -> chooseCharacter() choice
// ---------------------------------------------------------------------
int SaveManager::heroChoiceFromName(const string& name)
{
    if (name == "Dracula")       return 1;
    if (name == "sherlock")      return 2;
    if (name == "invisible man") return 3;
    return 0;
}

// ---------------------------------------------------------------------
// Player -> json
// ---------------------------------------------------------------------
json SaveManager::playerToJson(Player& player)
{
    json j;
    j["name"]         = player.getName();
    j["age"]           = player.getAge();
    j["fighterCount"]  = player.getfighterCount();

    Character* hero = player.getHero();
    j["heroChoice"] = heroChoiceFromName(hero->getName());
    j["owner"]      = hero->getowner();

    json chars = json::array();
    for (Character* ch : player.getCharacters())
        chars.push_back(characterToJson(ch));
    j["characters"] = chars;

    j["deck"] = deckToJson(player.getDeck());

    return j;
}

// json -> Player (rebuilds fighters/deck through the normal game API,
// then overwrites their state with what was saved)
void SaveManager::playerFromJson(Player& player, const json& j)
{
    player.setName(j.at("name").get<string>());
    player.setAge(j.at("age").get<int>());

    int choice    = j.at("heroChoice").get<int>();
    int ownerNum  = j.at("owner").get<int>();

    player.chooseCharacter(choice, ownerNum);
    player.setfighterCount(j.value("fighterCount", player.getfighterCount()));

    vector<Character*>& chars = player.getCharacters();
    const json& jchars = j.at("characters");

    for (size_t i = 0; i < chars.size() && i < jchars.size(); i++)
    {
        Character* ch      = chars[i];
        const json& jc     = jchars[i];

        ch->setHp(jc.at("hp").get<int>());
        ch->setSpace(jc.at("space").get<int>());

        if (jc.contains("mistTokens"))
        {
            if (invisible_man* im = dynamic_cast<invisible_man*>(ch))
            {
                const json& tokens = jc.at("mistTokens");
                for (size_t t = 0; t < tokens.size(); t++)
                    im->setMistToken(static_cast<int>(t), tokens[t].get<int>());
            }
        }
    }

    const json& jd = j.at("deck");
    vector<Card> deckCards, handCards, discardCards;

    for (const json& cj : jd.at("deckCards"))
        deckCards.push_back(cardFromJson(cj));
    for (const json& cj : jd.at("handCards"))
        handCards.push_back(cardFromJson(cj));
    for (const json& cj : jd.at("discardCards"))
        discardCards.push_back(cardFromJson(cj));

    player.getDeck()->loadState(deckCards, handCards, discardCards);
}

// ---------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------
bool SaveManager::saveGame(Player* current, Player* enemy, const string& filepath)
{
    if (!current || !enemy)
        return false;

    try
    {
        json root;
        root["currentOwner"] = current->getHero()->getowner();

        json players = json::array();
        players.push_back(playerToJson(*current));
        players.push_back(playerToJson(*enemy));
        root["players"] = players;

        ofstream file(filepath);
        if (!file.is_open())
            return false;

        file << root.dump(4);
        file.close();
        return true;
    }
    catch (const exception& e)
    {
        cerr << "SaveManager::saveGame failed: " << e.what() << endl;
        return false;
    }
}

bool SaveManager::loadGame(Bord& bord, Player players[2],
                            Player*& currentOut, Player*& enemyOut,
                            const string& filepath)
{
    try
    {
        ifstream file(filepath);
        if (!file.is_open())
            return false;

        json root;
        file >> root;
        file.close();

        if (!root.contains("players") || root["players"].size() < 2)
            return false;

        playerFromJson(players[0], root["players"][0]);
        playerFromJson(players[1], root["players"][1]);

        int currentOwner = root.value("currentOwner", 1);

        if (players[0].getHero()->getowner() == currentOwner)
        {
            currentOut = &players[0];
            enemyOut   = &players[1];
        }
        else
        {
            currentOut = &players[1];
            enemyOut   = &players[0];
        }

        // Re-place every living character on the board at its saved space.
        for (int p = 0; p < 2; p++)
        {
            for (Character* ch : players[p].getCharacters())
            {
                if (ch->checkalive() && ch->getSpace() != -1)
                    bord.addCharacter(ch->getSpace(), ch);
            }
        }

        return true;
    }
    catch (const exception& e)
    {
        cerr << "SaveManager::loadGame failed: " << e.what() << endl;
        return false;
    }
}