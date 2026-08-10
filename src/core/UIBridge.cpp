#include "core/UIBridge.hpp"

int UIBridge::requestInt()
{
    std::unique_lock<std::mutex> lock(mtx);
    pendingRequest = UIRequestType::Int;
    answerReady = false;
    cv.wait(lock, [this] { return answerReady; });
    pendingRequest = UIRequestType::None;
    return intAnswer;
}

int UIBridge::requestChoice(const std::vector<int>& valid)
{
    std::unique_lock<std::mutex> lock(mtx);
    pendingRequest = UIRequestType::Choice;
    pendingOptions = valid;
    answerReady = false;
    cv.wait(lock, [this] { return answerReady; });
    pendingRequest = UIRequestType::None;
    return intAnswer;
}

bool UIBridge::requestYesNo()
{
    std::unique_lock<std::mutex> lock(mtx);
    pendingRequest = UIRequestType::YesNo;
    answerReady = false;
    cv.wait(lock, [this] { return answerReady; });
    pendingRequest = UIRequestType::None;
    return boolAnswer;
}

std::string UIBridge::requestLine()
{
    std::unique_lock<std::mutex> lock(mtx);
    pendingRequest = UIRequestType::Line;
    answerReady = false;
    cv.wait(lock, [this] { return answerReady; });
    pendingRequest = UIRequestType::None;
    return stringAnswer;
}

void UIBridge::log(const std::string& msg)
{
    std::lock_guard<std::mutex> lock(mtx);
    logMessages.push_back(msg);
}

void UIBridge::setInPlayPhase(bool v)
{
    std::lock_guard<std::mutex> lock(mtx);
    inPlayPhase = v;
}

UIRequestType UIBridge::getPendingRequestType() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return pendingRequest;
}

std::vector<int> UIBridge::getPendingOptions() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return pendingOptions;
}

bool UIBridge::isInPlayPhase() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return inPlayPhase;
}

std::vector<std::string> UIBridge::drainLogs()
{
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> out = std::move(logMessages);
    logMessages.clear();
    return out;
}

void UIBridge::answerInt(int value)
{
    { std::lock_guard<std::mutex> lock(mtx); intAnswer = value; answerReady = true; }
    cv.notify_one();
}

void UIBridge::answerChoice(int value)
{
    { std::lock_guard<std::mutex> lock(mtx); intAnswer = value; answerReady = true; }
    cv.notify_one();
}

void UIBridge::answerYesNo(bool value)
{
    { std::lock_guard<std::mutex> lock(mtx); boolAnswer = value; answerReady = true; }
    cv.notify_one();
}

void UIBridge::answerLine(const std::string& value)
{
    { std::lock_guard<std::mutex> lock(mtx); stringAnswer = value; answerReady = true; }
    cv.notify_one();
}