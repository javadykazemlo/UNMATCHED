#pragma once
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>

// Bridge between the game-logic thread (Controller) and the GUI thread.
// Controller calls request*() and blocks until the GUI thread calls the
// matching answer*(). If no GUI is attached, Controller falls back to cin
// (see Controller.cpp), so the console mode keeps working untouched.
enum class UIRequestType { None, Int, Choice, YesNo, Line };

class UIBridge
{
private:
    mutable std::mutex mtx;
    std::condition_variable cv;

    UIRequestType pendingRequest = UIRequestType::None;
    std::vector<int> pendingOptions;   // valid values, only meaningful for Choice
    bool answerReady = false;
    int intAnswer = 0;
    bool boolAnswer = false;
    std::string stringAnswer;

    bool inPlayPhase = false;
    std::vector<std::string> logMessages;

public:
    // ---- called from the LOGIC thread (inside Controller) ----
    int requestInt();
    int requestChoice(const std::vector<int>& valid);
    bool requestYesNo();
    std::string requestLine();
    void log(const std::string& msg);
    void setInPlayPhase(bool v);

    // ---- called from the GUI thread every frame ----
    UIRequestType getPendingRequestType() const;
    std::vector<int> getPendingOptions() const;
    bool isInPlayPhase() const;
    std::vector<std::string> drainLogs();

    // one of these is called by the GUI once the user answered on screen
    void answerInt(int value);
    void answerChoice(int value);
    void answerYesNo(bool value);
    void answerLine(const std::string& value);
};