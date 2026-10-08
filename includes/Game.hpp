#pragma once

#include "raylib-cpp.hpp"
#include <iostream>
#include <memory>
#include <stack>
#include <thread>

#include "rlText.h"

#include "Game/GameLevel.hpp"
#include "Menu/LvlList.hpp"
#include "Utils/Task.hpp"

/*{ "name": "Forest Run", "path": "levels/lvl2.json" },
    { "name": "Mountain Peak", "path": "levels/lvl3.json" }*/

constexpr float INIT_WON_FONT_SIZE = 30;
constexpr float END_WON_FONT_SIZE = 60;

enum class Page {
    MENU,
    GAME
};

class Game
{
public:
    RWindow window;

    Game()
        : window(1200, 700, "Word Builder"),
        m_camera2D(),
        m_gameZone({(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2, (float)GetScreenWidth() + 500, (float)GetScreenHeight()}),
        m_lvlMenu("levels.json")
    {
        window.SetTargetFPS(144);
        initCamera();

        m_logicFunctions[Page::MENU] = [this]() {
            MenuLevel* selectedLvl = m_lvlMenu.update(); 
            if (selectedLvl) {
                loadLevel(*selectedLvl);
            }
        };

        m_logicFunctions[Page::GAME] = [this]() {
            if (m_gameLvl) {
                m_gameLvl->update();
                
                if (IsKeyPressed(KEY_E) && m_gameLvl->getState() == GameLvlState::PLAYING) {
                    quitLevel();
                }
            }
        };

        // --- DRAW MAPPING ---
        m_drawFunctions[Page::MENU] = [this]() {
            window.ClearBackground(RAYWHITE);
            m_lvlMenu.draw(); 
        };

        m_drawFunctions[Page::GAME] = [this]() {
            m_camera2D.BeginMode();
            if (m_gameLvl)
                m_gameLvl->draw();
            m_camera2D.EndMode();

            if (m_gameLvl)
            {
                m_gameLvl->drawUI();

                if (m_gameLvl->getState() == GameLvlState::WON || m_gameLvl->getState() == GameLvlState::LOST)
                {
                    m_wonTimer += GetFrameTime();
                    if (m_wonTimer <= 0.5f)
                    {
                        float diff = END_WON_FONT_SIZE - INIT_WON_FONT_SIZE;
                        m_wonFontSize = EaseSineInOut(m_wonTimer, INIT_WON_FONT_SIZE, diff, 0.5f);
                    }
                }
                else
                {
                    DrawText("Press E to exit", 10, GetScreenHeight() - 25, 15, Color{0, 0, 0, 70});
                }

                switch (m_gameLvl->getState())
                {
                case GameLvlState::WON:
                    rltDrawTextJustified(TextFormat("Good job ! %d points won", m_gameLvl->getScore()), m_wonFontSize, (window.GetSize() / 2) - RVector2{0, m_wonFontSize/2}, Color{0, 161, 28, 255}, rltAllignment::Center);
                    break;
                case GameLvlState::LOST:
                    m_wonTimer += GetFrameTime();
                    rltDrawTextJustified("You lost", m_wonFontSize, (window.GetSize() / 2) - RVector2{0, m_wonFontSize/2}, Color{161, 0, 0, 255}, rltAllignment::Center);
                    break;
                }
            }
        };

        m_pageStack.push(Page::MENU);
    }

    void doLogic()
    {
        Task::update();
        if (!m_pageStack.empty()) {
            Page currentPage = m_pageStack.top();
            if (m_logicFunctions.count(currentPage)) {
                m_logicFunctions[currentPage]();
            }
        }
    }

    void drawAll()
    {
        window.BeginDrawing();
        window.ClearBackground(BLACK);

        // Execute the draw function for the current page
        if (!m_pageStack.empty()) {
            Page currentPage = m_pageStack.top();
            if (m_drawFunctions.count(currentPage)) {
                m_drawFunctions[currentPage]();
            }
        }
        
        window.DrawFPS(10, 35);
        window.EndDrawing();
    }

private:
    RCamera2D m_camera2D;
    RectanglePro m_gameZone;

    std::stack<Page> m_pageStack;
    std::unordered_map<Page, std::function<void()>> m_logicFunctions;
    std::unordered_map<Page, std::function<void()>> m_drawFunctions;
    
    LvlList m_lvlMenu;
    std::unique_ptr<GameLevel> m_gameLvl;
    Connection m_stateChangedConn;

    float m_wonTimer = 0;
    float m_wonFontSize = INIT_WON_FONT_SIZE;

    void loadLevel(const MenuLevel& lvlMenu)
    {
        LevelData data = lvlMenu.createLevelData();
        m_gameLvl.reset(new GameLevel(std::move(data), m_camera2D, m_gameZone));
        m_pageStack.push(Page::GAME);
        window.HideCursor();

        m_stateChangedConn = m_gameLvl->onStateChanged.connect([this](GameLvlState state) {
            if (state == GameLvlState::WON) {
                User::getInstance().onLevelComplete(m_gameLvl->getLevel(), m_gameLvl->getScore());
            }

            if (state == GameLvlState::WON || state == GameLvlState::LOST)
            {
                Task::delay(4.f, [this]() {
                    this->quitLevel();
                });

                this->m_stateChangedConn.disconnect();
            }
        });
    }

    void quitLevel()
    {
        this->m_wonTimer = 0;
        window.ShowCursor();
        if (!this->m_pageStack.empty())
        {
            this->m_pageStack.pop();
        }
    }

    void initCamera() {
        m_camera2D.target = {600, 350};
        m_camera2D.offset = {600, 350};
        m_camera2D.rotation = 0.f;
        m_camera2D.zoom = 1.0f;
        m_gameZone.origin.x = m_gameZone.rect.GetWidth() / 2;
        m_gameZone.origin.y = m_gameZone.rect.GetHeight() / 2;
    }
};
