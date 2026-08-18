#include "core/GameAI.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <streambuf>
#include <string>
#include <thread>

#include "core/Controller.hpp"
#include "core/Player.hpp"
#include "entities/Character.hpp"
#include "cards/Card.hpp"
#include "cards/Deck.hpp"

using std::cout;
using std::endl;

namespace
{

    class GuiCoutRedirect
    {
    public:
        explicit GuiCoutRedirect(Controller& owner)
            : buf_(owner), previous_(std::cout.rdbuf(&buf_))
        {
        }

        ~GuiCoutRedirect()
        {
            std::cout.rdbuf(previous_);
        }

        GuiCoutRedirect(const GuiCoutRedirect&) = delete;
        GuiCoutRedirect& operator=(const GuiCoutRedirect&) = delete;

    private:
        class LineBuf : public std::streambuf
        {
        public:
            explicit LineBuf(Controller& owner) : owner_(owner) {}

        protected:
            int overflow(int ch) override
            {
                if (ch != EOF)
                {
                    if (ch == '\n')
                        flush();
                    else
                        line_ += static_cast<char>(ch);
                }
                return ch;
            }

            int sync() override
            {
                flush();
                return 0;
            }

        private:
            void flush()
            {
                if (!line_.empty())
                {
                    owner_.guiLogEffect(line_);
                    line_.clear();
                }
            }

            Controller& owner_;
            std::string line_;
        };

        LineBuf buf_;
        std::streambuf* previous_;
    };
}

GameAI& GameAI::operator=(const GameAI& other)
{
    if (this != &other)
    {
        decisionKind = other.decisionKind;
        cardPurpose = other.cardPurpose;
        characterOptions = other.characterOptions;
        movingCharacter = other.movingCharacter;
        cardFighter = other.cardFighter;
        turnBusy.store(other.turnBusy.load());
    }
    return *this;
}

int GameAI::scoreAction(Controller& controller, int action, Player* decider)
{
    bool canAttackAny = false;
    for (Character* ch : decider->getCharacters())
        if (ch && ch->checkalive() && controller.bord.canAttack(ch->getAttacktype(), ch->getSpace()))
            canAttackAny = true;

    bool hasAttackCard = !decider->getDeck()->getAttackCardIndices().empty();
    bool hasSchemeCard = !decider->getDeck()->getSchemeCardIndices().empty();

    switch (action)
    {
        case 3:
            return (canAttackAny && hasAttackCard) ? 100 : -50;
        case 2:
            return hasSchemeCard ? 55 : -50;
        case 1:
            return 40;
        case 4:
            return 5;
    }
    return 0;
}

int GameAI::scoreFighter(int idx, Player* /*decider*/)
{
    if (idx < 1 || idx > static_cast<int>(characterOptions.size()))
        return -1000;

    Character* ch = characterOptions[idx - 1];
    if (!ch) return -1000;

    int score = ch->getHp();
    if (ch->isHero())
        score += 5;
    return score;
}

int GameAI::scoreMove(Controller& controller, int destination, Player* decider)
{
    Player* opp = (decider == controller.current) ? controller.enemy : controller.current;
    Character* mover = movingCharacter;
    Character* enemyHero = opp ? opp->getHero() : nullptr;

    if (!mover || !enemyHero || enemyHero->getSpace() == -1)
        return 0;

    int dist = controller.boardDistance(destination, enemyHero->getSpace());
    bool lowHp = mover->getHp() <= mover->getMaxhp() / 3;

    if (lowHp)
        return std::min(dist, 6) * 10;

    int score = 100 - std::min(dist, 10) * 8;
    if (controller.bord.canAttack(mover->getAttacktype(), destination))
        score += 30;

    return score;
}

int GameAI::scoreAttackTarget(int idx, Player* /*decider*/)
{
    if (idx < 1 || idx > static_cast<int>(characterOptions.size()))
        return -1000;

    Character* target = characterOptions[idx - 1];
    if (!target) return -1000;

    int score = 0;
    if (target->isHero())
        score += 60;
    score += (target->getMaxhp() - target->getHp()) * 3;
    score += std::max(0, 20 - target->getHp());

    return score;
}

int GameAI::scoreCardChoice(int idx, Player* decider)
{
    const std::vector<Card>& hand = decider->getDeck()->gethand();
    if (idx < 1 || idx > static_cast<int>(hand.size()))
        return -1000;

    const Card& c = hand[idx - 1];

    if (cardFighter)
    {
        bool ownerOK = cardFighter->isHero() ? (c.isHero() || c.isAnyowner())
                                              : (c.issideKick() || c.isAnyowner());
        if (!ownerOK)
            return -1000;

        if (cardPurpose == CardPurpose::Attack && !(c.isAttack() || c.isVersatile()))
            return -1000;
        if (cardPurpose == CardPurpose::Defense && !(c.isDefense() || c.isVersatile()))
            return -1000;
    }

    switch (cardPurpose)
    {
        case CardPurpose::Attack:
            return c.getAttack() * 10;
        case CardPurpose::Defense:
            return c.getAttack() * 10;
        case CardPurpose::Boost:
            return c.getBoost() * 6 - c.getAttack() * 4;
        case CardPurpose::Scheme:
            return c.getBoost() * 8 + c.getAttack() * 2;
    }
    return 0;
}

void GameAI::think(Player* decider)
{

    int ms = 2500 + (rand() % 1500);
    int dots = 4;
    for (int i = 0; i < dots; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms / dots));
    }
    cout << "\n";
}

int GameAI::choose(Controller& controller, const std::vector<int>& valid, Player* decider)
{
    if (valid.empty())
        return 0;

    think(decider);

    int bestScore = std::numeric_limits<int>::min();
    std::vector<int> best;

    for (int v : valid)
    {
        int score;
        switch (decisionKind)
        {
            case Decision::ActionChoice:    score = scoreAction(controller, v, decider);   break;
            case Decision::FighterSelect:   score = scoreFighter(v, decider);              break;
            case Decision::MoveDestination: score = scoreMove(controller, v, decider);     break;
            case Decision::AttackTarget:    score = scoreAttackTarget(v, decider);         break;
            case Decision::CardSelect:      score = scoreCardChoice(v, decider);           break;
            default:                        score = rand() % 100;                          break;
        }

        if (score > bestScore)
        {
            bestScore = score;
            best.clear();
            best.push_back(v);
        }
        else if (score == bestScore)
        {
            best.push_back(v);
        }
    }

    int pick = best[rand() % best.size()];
    cout << decider->getName() << " chooses: " << pick << "\n";
    return pick;
}

bool GameAI::yesNo(Controller& /*controller*/, Player* decider)
{
    think(decider);

    bool yes;

    if (decisionKind == Decision::BoostChoice)
    {
        const std::vector<Card>& hand = decider->getDeck()->gethand();
        bool handFull = hand.size() >= 6;
        bool hasSpareCard = false;
        for (const Card& c : hand)
            if (c.getAttack() <= 1) { hasSpareCard = true; break; }

        yes = handFull || hasSpareCard;
    }
    else
    {
        yes = (rand() % 2) == 0;
    }

    cout << decider->getName() << " answers: " << (yes ? "yes" : "no") << "\n";
    return yes;
}

int GameAI::chooseInt(Controller& /*controller*/, Player* decider)
{
    think(decider);

    int x = rand() % 7 + 1;
    cout << decider->getName() << " enters: " << x << "\n";
    return x;
}

bool GameAI::guiStartTurn(Controller& controller)
{
    if (!controller.current || !controller.current->isAI()) return false;
    if (turnBusy.load()) return false;
    if (controller.guiEffectBusy()) return false;

    turnBusy = true;

    std::thread([this, &controller]()
    {
        runTurn(controller);
    }).detach();

    return true;
}

bool GameAI::guiTurnBusy() const
{
    return turnBusy.load();
}

void GameAI::runTurn(Controller& controller)
{
    if (!controller.current || !controller.current->isAI())
    {
        turnBusy = false;
        return;
    }

    controller.guiMode = true;
    controller.activeDecider = controller.current;

    {
        GuiCoutRedirect redirect(controller);

        try
        {
            controller.guiLogEffect(controller.current->getName() + " (AI) is taking its turn...");

            if (controller.current->getHero() && controller.current->getHero()->getName() == "Dracula")
                controller.current->getHero()->ability(controller.bord, controller.current);

            while (controller.gamerand < 2 && !controller.isGameOver())
            {
                controller.playSingleTurn(false);
            }

            if (!controller.isGameOver())
                controller.performHandLimitDiscard();
        }
        catch (const std::exception& e)
        {
            controller.guiLogEffect(std::string("AI turn error: ") + e.what());
        }
        catch (...)
        {
            controller.guiLogEffect("AI turn error: unknown exception.");
        }
    }

    // Keep the AI player as the current player until the user presses END TURN.
    // The GUI shows a summary of this turn before advancing to the next player.
    turnBusy = false;
}