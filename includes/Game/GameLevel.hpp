#pragma once

#include <vector>
#include <raylib-cpp.hpp>

#include "WordBuilder.hpp"
#include "Utils/Strings.hpp"
#include "Utils/Signal.hpp"

#include "LevelData.hpp"
#include "rlText.h"

enum class GameLvlState
{
    PLAYING,
    WON,
    LOST
};

class GameLevel {
public:
    Signal<GameLvlState> onStateChanged;

    GameLevel(
        LevelData&& data,
        RCamera2D& camera,
        const RectanglePro& gameZone
    ) : m_data(std::move(data)),
        m_camera(camera),
        m_gameZone(gameZone),
        m_builder(*m_data.strategyMap.getAt(0), m_data.text.get(0, 0))
    {
        m_currentDiff = m_data.diffMap.getAt(0);
    }

    void update() {
        //if (getState() != GameLvlState::PLAYING) return;

        float dt = GetFrameTime();
        m_spawnTimer += dt;
        m_cameraTimer += dt;

        // 1. Camera Smoothing
        if (m_cameraTimer <= 3.0f) {
            float totalChange = m_currentDiff->cameraRot - m_startCamRot;
            m_camera.rotation = EaseSineInOut(m_cameraTimer, m_startCamRot, totalChange, 3.0f);
        }

        // Update Word
        RVector2 worldMouse = m_camera.GetScreenToWorld(GetMousePosition());
        m_builder.update(worldMouse, m_camera.rotation);

        //
        auto it = m_letters.begin();
        while (it != m_letters.end()) {
            it->updatePhysics();

            if ((it->pos - worldMouse).Length() > 2000.0f)
            {
                it = m_letters.erase(it);
            }
            else 
            {
                ++it;
            }
        }

        // Spawning
        if (m_spawnTimer > m_currentDiff->letterTimerOut) {
            m_spawnTimer = 0;
            m_builder.spawnLetter(m_letters, m_gameZone, *m_currentDiff);
        }

        // 4. Collision & Progression
        for (int i = 0; i < m_letters.size(); i++) {
            auto result = m_builder.handleCollision(m_letters[i].getCollisionPos(), m_letters[i].getCollisionRadius(), m_letters[i].getLetter());

            if (result == LetterCollisionResult::CORRECT || result == LetterCollisionResult::INCORRECT) {
                m_letters.erase(m_letters.begin() + i);
                i--;

                if (result == LetterCollisionResult::CORRECT) {
                    m_score++;

                    if (m_builder.isComplete()) {
                        m_wordIdx++;
                        m_wordScore++;

                        m_startCamRot = m_camera.rotation;
                        m_cameraTimer = 0;

                        DifficultyProfile* nextDiff = m_data.diffMap.getAt(m_wordScore);
                        if (nextDiff)
                            m_currentDiff = nextDiff;

                        if (m_wordIdx >= m_data.text.getLayer(m_sentenceIdx).size())
                        {
                            m_sentenceIdx++;
                            m_wordIdx = 0;
                        }

                        if (m_sentenceIdx < m_data.text.size())
                        {
                            auto* nextStrategy = m_data.strategyMap.getAt(m_wordScore);
                            m_builder.reset(m_data.text.getLayer(m_sentenceIdx).get(m_wordIdx), *nextStrategy);
                        }
                    }
                }
                else if (result == LetterCollisionResult::INCORRECT)
                {
                    m_data.lifes--;
                }
                
                onStateChanged.fire(getState());
            }
        }
    }

    void draw() {
        m_gameZone.draw(RAYWHITE);
        m_builder.draw();
        for (auto& l : m_letters)
        {
            l.draw();
        }
    }

    void drawUI()
    {
        DrawText(TextFormat("Lifes: %d", m_data.lifes), 10, 10, 20, RED);

        int fontSize = m_builder.word.fontSize + 4;
        int posY = 20;

        if (m_sentenceIdx == 0 && m_wordIdx == 0)
        {
            rltDrawTextJustified(m_data.firstText, fontSize, {(float)GetScreenWidth() / 2, (float)posY}, GRAY, rltAllignment::Center);
        }
        else
        {
            int maxWordId = m_wordIdx;
            int sentenceId = m_sentenceIdx;
            if (m_wordIdx == 0 && sentenceId != 0)
            {
                sentenceId = sentenceId - 1;
                maxWordId = m_data.text.getLayer(sentenceId).size();
            }

            String2D& currentSentence = m_data.text.getLayer(sentenceId);
            std::string& data = currentSentence.getData();
            char* currentStart = data.data();
            int currentIdx = 0;

            int textSize = 0;
            int beginI = 0;
            int spaceSize = MeasureText(" ", fontSize);

            for (int endI = 0; endI < maxWordId; endI++)
            {
                std::string_view word = currentSentence.get(endI);
                
                int wWidth = MeasureText(TextFormat("%.*s", (int)word.size(), word.data()), fontSize);
                textSize += wWidth + spaceSize;

                if (textSize > GetScreenWidth() - 200 || endI == maxWordId - 1)
                {
                    int posX = (GetScreenWidth() - textSize) / 2;

                    for (int i = beginI; i <= endI; i++)
                    {
                        std::string_view w = currentSentence.get(i);
                        if (i != beginI) posX += spaceSize;
                        
                        DrawText(TextFormat("%.*s", (int)w.size(), w.data()), posX, posY, fontSize, GRAY);

                        posX += MeasureText(TextFormat("%.*s", (int)w.size(), w.data()), fontSize);
                    }

                    textSize = 0;
                    posY += fontSize;
                    beginI = endI + 1;
                }
            }
        }
    }

    GameLvlState getState()
    {
        if (m_data.lifes <= 0)
            return GameLvlState::LOST;
        if (m_sentenceIdx >= m_data.text.size())
        {
            return GameLvlState::WON;
        }
            
        return GameLvlState::PLAYING;
    }

    int getScore()
    {
        return m_score;
    }

    int getLevel()
    {
        return m_data.level;
    }

private:
    LevelData m_data;

    int m_score = 0;
    int m_wordScore = 0;
    int m_wordIdx = 0;
    int m_sentenceIdx = 0;

    std::vector<MoveLetter> m_letters;
    WordBuilder m_builder;
    RCamera2D& m_camera;
    RectanglePro m_gameZone;

    DifficultyProfile* m_currentDiff;

    float m_spawnTimer = 0;
    float m_cameraTimer = 0;
    float m_startCamRot = 0;
};