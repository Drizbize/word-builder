#pragma once

#include <string>
#include <vector>
#include <json.hpp>
#include <raylib-cpp.hpp>

#include "MenuLevel.hpp"
#include "Entity/User.hpp" // Ensure this points to your User singleton class

using json = nlohmann::json;

class LvlList {
private:
    // Layout constants: We define them here so both update() and draw() use the exact same positions
    const int startX = 100;
    const int startY = 150;
    const int padding = 60;
    const int buttonWidth = 400;
    const int buttonHeight = 30;

public:
    std::vector<MenuLevel> lvls;

    LvlList(std::string lvlsJsonPath) {
        std::ifstream file(lvlsJsonPath);
        if (file.is_open()) {
            json j;
            file >> j;

            int lvl = 1;
            for (auto& item : j) {
                MenuLevel m;
                m.name = item["name"];
                m.lvlDataJsonPath = item["path"];
                //m.numberLetters = LevelData::GetLetterCount(item["path"]);
                m.lvl = lvl++;

                lvls.push_back(m);
            }
        }
    }

    // LOGIC: Checks for inputs and returns the path if a level is clicked
    MenuLevel* update() {
        User& user = User::getInstance();

        for (int i = 0; i < lvls.size(); i++) {
            if (lvls[i].lvl > user.getLevel()) continue;
            
            Rectangle buttonRect = {
                (float)startX, 
                (float)startY + (i * padding), 
                (float)buttonWidth, 
                (float)buttonHeight
            };

            if (CheckCollisionPointRec(GetMousePosition(), buttonRect)) {
                // 3. Input Check: Did they click?
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    return &lvls[i]; // Return the specific JSON path
                }
            }
        }

        return nullptr;
    }

    // RENDER: Only draws the UI
    void draw() {
        User& user = User::getInstance(); // Access the singleton for display data
        
        const char* title = "SELECT LEVEL";
        DrawText(title, GetScreenWidth() / 2 - MeasureText(title, 20), 50, 40, DARKGRAY);
        DrawText(TextFormat("Total Score: %i", user.getScore()), 10, 15, 20, BLACK);

        for (int i = 0; i < lvls.size(); i++) {
            int levelNum = lvls[i].lvl;
            bool isUnlocked = levelNum <= user.getLevel();
            
            Color textColor = isUnlocked ? BLACK : LIGHTGRAY;
            
            std::string label = TextFormat("Lvl %i: %s", levelNum, lvls[i].name.c_str());
            if (!isUnlocked) label += " [LOCKED]";

            int posY = startY + (i * padding);
            DrawText(label.c_str(), startX, posY, 30, textColor);

            if (isUnlocked) {
                Rectangle buttonRect = {
                    (float)startX, 
                    (float)posY, 
                    (float)buttonWidth, 
                    (float)buttonHeight
                };

                if (CheckCollisionPointRec(GetMousePosition(), buttonRect)) {
                    DrawRectangleLines(startX - 5, posY - 2, 600, 35, RED);
                }
            }
        }
    }
};