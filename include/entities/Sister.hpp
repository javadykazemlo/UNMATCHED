#pragma once
#include <string>
#include "Character.hpp"

class Sister : public Character
{
private:
    // std::string name;
public:
    Sister(std::string name);

    void showSister() const;

    void ability() override;

    ~Sister();
};