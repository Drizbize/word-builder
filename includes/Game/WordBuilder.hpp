#pragma once

#include <vector>
#include <map>
#include <raylib-cpp.hpp>

#include "Entity/MoveLetter.hpp"
#include "Entity/Word.hpp"
#include "reasings.h"

#include "LevelData.hpp"

enum class LetterCollisionResult
{
    NONE,
    CORRECT,
    INCORRECT
};

class WordBuilder {
public:
    Word word;

    WordBuilder(const WordGameStrategy& strategy, const std::string_view& text)
        : word(strategy, text, 40, {0,0})
    {
        word.origin = RVector2{0.5f, 0.5f};
    }

    void reset(const std::string_view& text, const WordGameStrategy& strategy)
    {
        word = Word(strategy, text, 40, {0, 0});
        word.origin = RVector2{0.5f, 0.5f};
    }

    void update(const RVector2& worldMousePos, float cameraRotation)
    {
        word.position = worldMousePos;
        word.rotation = -cameraRotation; // Counter-rotate so word stays level
    }

    void spawnLetter(std::vector<MoveLetter>& letters, const RectanglePro& gameZone, const DifficultyProfile& activeDifficulty)
    {
        std::vector<char> targets = word.getActiveTargetLetters(); 
        if (targets.empty()) return;

        char charToSpawn = targets[GetRandomValue(0, targets.size() - 1)];

        auto zoneVerts = gameZone.getVertices();
        int side = GetRandomValue(0, 1);
        RVector2 p1 = zoneVerts[2 * side];
        RVector2 p2 = zoneVerts[2 * side + 1];

        float interval = activeDifficulty.spawnRange * 50.f;
        float lerp = (float)GetRandomValue(50 - interval, 50 + interval) / 100.0f;
        RVector2 spawnPos = { 
            p1.x + (p2.x - p1.x) * lerp,
            p1.y + (p2.y - p1.y) * lerp 
        };

        RVector2 targetPos = gameZone.rect.GetPosition() + RVector2{(float)GetRandomValue(-200, 200), 0};
        RVector2 dirOut = (spawnPos - targetPos).Normalize();
        spawnPos += dirOut * (float)GetRandomValue(10, 30);

        letters.emplace_back(charToSpawn, 40, spawnPos, activeDifficulty.letterMoveType); 
        RVector2 velocity = (targetPos - spawnPos).Normalize() * (float)GetRandomValue(activeDifficulty.velocityMin, activeDifficulty.velocityMax);
        letters.back().setVelocity(velocity);
        letters.back().rotVelocity = (float)GetRandomValue(-activeDifficulty.velocityRot, activeDifficulty.velocityRot);
    }

    // Returns true if a letter was successfully played
    LetterCollisionResult handleCollision(const RVector2& pos, float radius, char letter)
    {
        int idx = word.getCollisionLetterIndex(pos, radius);
        if (idx != -1)
        {
            if (word.play(idx, letter))
            {
                return LetterCollisionResult::CORRECT;
            }
            else
            {
                return LetterCollisionResult::INCORRECT;
            }
        }

        return LetterCollisionResult::NONE;
    }

    void draw()
    {
        word.draw();
    }

    bool isComplete() const
    {
        return word.isSolved();
    }
private:
    
};