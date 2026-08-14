#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <system_error>

#include "Save/SaveManager.hpp"
#include "entities/invisible_man.hpp"

using json = nlohmann::json;
using namespace std;

namespace
{
    string formatDate(time_t tt)
    {
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &tt);
#else
        localtime_r(&tt, &tm);
#endif
        ostringstream out;
        out << put_time(&tm, "%Y-%m-%d");
        return out.str();
    }

    string formatTime(time_t tt)
    {
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &tt);
#else
        localtime_r(&tt, &tm);
#endif
        ostringstream out;
        out << put_time(&tm, "%H:%M:%S");
        return out.str();
    }

    string createUniquePath(const string& directory)
    {
        namespace fs = std::filesystem;
        fs::create_directories(directory);

        const auto now = chrono::system_clock::now();
        const time_t tt = chrono::system_clock::to_time_t(now);
        const auto millis = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

        string base = directory + "/save_" + formatDate(tt) + "_" + formatTime(tt);
        ostringstream suffix;
        suffix << '_' << setw(3) << setfill('0') << millis;

        fs::path candidate = base + suffix.str() + ".json";
        int counter = 1;
        while (fs::exists(candidate))
            candidate = base + suffix.str() + "_" + to_string(counter++) + ".json";

        return candidate.string();
    }

    bool isLegacyOrSaveFile(const std::filesystem::directory_entry& entry)
    {
        return entry.is_regular_file() && entry.path().extension() == ".json";
    }
}

std::string SaveManager::createSavePath(const std::string& directory)
{
    try
    {
        return createUniquePath(directory);
    }
    catch (...)
    {
        return {};
    }
}

std::vector<SaveManager::SaveInfo> SaveManager::listSaves(const std::string& directory)
{
    namespace fs = std::filesystem;
    vector<SaveInfo> result;

    try
    {
        if (!fs::exists(directory))
            return result;

        for (const auto& entry : fs::directory_iterator(directory))
        {
            if (!isLegacyOrSaveFile(entry))
                continue;

            try
            {
                ifstream file(entry.path());
                if (!file.is_open())
                    continue;

                json root;
                file >> root;
                if (!root.contains("players") || !root["players"].is_array() || root["players"].size() < 2)
                    continue;

                SaveInfo info;
                info.filepath = entry.path().string();

                if (root.contains("saveMetadata") && root["saveMetadata"].is_object())
                {
                    const json& meta = root["saveMetadata"];
                    info.date = meta.value("date", string{});
                    info.time = meta.value("time", string{});
                    info.timestamp = meta.value("timestamp", static_cast<int64_t>(0));
                }

                if (info.timestamp == 0)
                {
                    const auto ft = fs::last_write_time(entry.path());
                    const auto adjusted = chrono::time_point_cast<chrono::system_clock::duration>(
                        ft - fs::file_time_type::clock::now() + chrono::system_clock::now());
                    const auto tt = chrono::system_clock::to_time_t(adjusted);
                    info.timestamp = static_cast<int64_t>(tt);
                    info.date = formatDate(tt);
                    info.time = formatTime(tt);
                }

                if (info.date.empty() || info.time.empty())
                {
                    const time_t tt = static_cast<time_t>(info.timestamp);
                    info.date = formatDate(tt);
                    info.time = formatTime(tt);
                }

                const string p0 = root["players"][0].value("name", string{"Player 1"});
                const string p1 = root["players"][1].value("name", string{"Player 2"});
                string h0;
                string h1;
                if (root["players"][0].contains("characters") && !root["players"][0]["characters"].empty())
                    h0 = root["players"][0]["characters"][0].value("name", string{});
                if (root["players"][1].contains("characters") && !root["players"][1]["characters"].empty())
                    h1 = root["players"][1]["characters"][0].value("name", string{});

                info.players = p0 + " vs " + p1;
                if (!h0.empty() && !h1.empty())
                    info.players += "  |  " + h0 + " vs " + h1;

                result.push_back(std::move(info));
            }
            catch (...)
            {
                // Ignore only this broken save; keep the rest of the list usable.
            }
        }

        sort(result.begin(), result.end(), [](const SaveInfo& a, const SaveInfo& b)
        {
            if (a.timestamp != b.timestamp)
                return a.timestamp > b.timestamp;
            return a.filepath > b.filepath;
        });
    }
    catch (...)
    {
        result.clear();
    }

    return result;
}

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

    if (invisible_man* im = dynamic_cast<invisible_man*>(character))
    {
        json tokens = json::array();
        for (int pos : im->getMistTokens())
            tokens.push_back(pos);
        j["mistTokens"] = tokens;
    }

    return j;
}


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


int SaveManager::heroChoiceFromName(const string& name)
{
    if (name == "Dracula")       return 1;
    if (name == "sherlock")      return 2;
    if (name == "invisible man") return 3;
    return 0;
}


json SaveManager::playerToJson(Player& player)
{
    json j;
    j["name"]         = player.getName();
    j["age"]           = player.getAge();
    j["fighterCount"]  = player.getfighterCount();
    j["ai"]            = player.isAI();

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


void SaveManager::playerFromJson(Player& player, const json& j)
{
    player.setName(j.at("name").get<string>());
    player.setAge(j.at("age").get<int>());
    player.setAI(j.value("ai", false));

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


bool SaveManager::saveGame(Player* current, Player* enemy,
                            int gamerand, bool cancelEffectDR, bool cancelEffectSH,
                            bool cancelEffectIM, bool guessElementary,
                            const string& filepath)
{
    if (!current || !enemy)
        return false;

    try
    {
        json root;
        root["currentOwner"] = current->getHero()->getowner();

        root["gamerand"]        = gamerand;
        root["cancelEffectDR"]  = cancelEffectDR;
        root["cancelEffectSH"]  = cancelEffectSH;
        root["cancelEffectIM"]  = cancelEffectIM;
        root["guessElementary"] = guessElementary;

        json players = json::array();
        players.push_back(playerToJson(*current));
        players.push_back(playerToJson(*enemy));
        root["players"] = players;

        const auto now = chrono::system_clock::now();
        const time_t tt = chrono::system_clock::to_time_t(now);
        const string actualPath = (filepath.empty() || filepath == "save.json")
            ? createUniquePath("saves")
            : filepath;
        if (actualPath.empty())
            return false;

        root["saveMetadata"] = {
            {"id", filesystem::path(actualPath).stem().string()},
            {"date", formatDate(tt)},
            {"time", formatTime(tt)},
            {"timestamp", static_cast<int64_t>(tt)}
        };

        if (const filesystem::path parent = filesystem::path(actualPath).parent_path(); !parent.empty())
            filesystem::create_directories(parent);

        ofstream file(actualPath);
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
                            int& gamerandOut, bool& cancelEffectDROut, bool& cancelEffectSHOut,
                            bool& cancelEffectIMOut, bool& guessElementaryOut,
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

        for (int i = 0; i < 32; ++i)
        {
            if (!bord.isEmpty(i))
                bord.deletCharacter(i);
        }
        players[0].reset();
        players[1].reset();

        playerFromJson(players[0], root["players"][0]);
        playerFromJson(players[1], root["players"][1]);

        gamerandOut        = root.value("gamerand", 0);
        cancelEffectDROut  = root.value("cancelEffectDR", false);
        cancelEffectSHOut  = root.value("cancelEffectSH", false);
        cancelEffectIMOut  = root.value("cancelEffectIM", false);
        guessElementaryOut = root.value("guessElementary", false);

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

        for (int p = 0; p < 2; p++)
        {
            for (Character* ch : players[p].getCharacters())
            {
                if (ch->checkalive() && ch->getSpace() >= 0 && ch->getSpace() < 32)
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