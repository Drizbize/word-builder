#pragma once

#include <string>
#include "Game/LevelData.hpp"

class MenuLevel
{
public:
    std::string name;
    //int numberLetters;
    int lvl;
    std::string lvlDataJsonPath;

    MenuLevel() = default;
    ~MenuLevel() = default;

    LevelData createLevelData() const
    {
        return LevelData::LoadFromFile(lvlDataJsonPath, lvl);
    }
private:
};

