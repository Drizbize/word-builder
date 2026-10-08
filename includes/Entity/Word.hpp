#pragma once

#include <string>
#include <list>
#include <raylib-cpp.hpp>

#include "WordLetter.hpp"
#include "Game/WordGameStrategy.hpp"

class Word
{
public:
    float rotation;
    RVector2 position;
    RVector2 origin;
    float spacing;
    float fontSize;
    Font font = GetFontDefault();

    Word(const WordGameStrategy& strategy, const std::string_view& text, float fontSize, RVector2 pos, float spacing = 7.0f, float rotation = 0.f)
        :m_strategy(strategy), rotation(rotation), position(pos), origin(0, 0), spacing(spacing), fontSize(fontSize) 
    {
        for (int i = 0; i < text.length(); i++)
        {
            m_word.emplace_back(text[i]);
        }

        m_strategy.initWord(m_word);
    }

    void draw() {
        if (m_word.empty()) return;

        RFont font = GetFontDefault();
        float rad = rotation * DEG2RAD;
        
        RVector2 pivot = getPivotOffsets();
        float currentX = 0.0f;

        for (size_t i = 0; i < m_word.size(); i++) {
            float charWidth = measureChar(m_word[i].letter);
            RVector2 localPos = { currentX - pivot.x, 0.0f - pivot.y };
            RVector2 finalPos = position + localPos.Rotate(rad);

            drawLetter(i, finalPos);

#ifdef DEBUG
            if (m_word[i].letter != ' ')
            {
                RVector2 localCenter = { 
                    (currentX + charWidth / 2.0f) - pivot.x, 
                    (fontSize / 2.0f) - pivot.y
                };
                RVector2 globalCenter = position + localCenter.Rotate(rotation * DEG2RAD);

                DrawCircleLinesV(globalCenter, fontSize / 4, RED);
                DrawCircleV(globalCenter, 2, BLUE);
            }
#endif

            currentX += charWidth + spacing;
        }
    }

    int getCollisionLetterIndex(RVector2 circlePos, float radius) {
        if (m_word.empty()) return -1;

        RVector2 pivot = getPivotOffsets();
        float currentX = 0.0f;
        float letterRadius = fontSize / 4;

        for (size_t i = 0; i < m_word.size(); i++) {
            float charWidth = measureChar(m_word[i].letter);
            if (m_word[i].letter != ' ' && m_word[i].style == WordLetterStyle::COLLIDEABLE && m_word[i].state != WordLetterState::SOLVED)
            {
                RVector2 localCenter = { 
                    (currentX + charWidth / 2.0f) - pivot.x, 
                    (fontSize / 2.0f) - pivot.y 
                };
                
                RVector2 globalCenter = position + localCenter.Rotate(rotation * DEG2RAD);

                if (CheckCollisionCircles(globalCenter, letterRadius, circlePos, radius)) {
                    return (int)i;
                }
            }
            
            currentX += charWidth + spacing;
        }
        return -1;
    }

    bool play(int index, char letter)
    {
        return m_strategy.setLetter(m_word, index, letter);
    }

    std::vector<char> getActiveTargetLetters() {
        std::vector<char> active;
        for (const auto& wl : m_word) {
            if (wl.state == WordLetterState::KNOWN && wl.style == WordLetterStyle::COLLIDEABLE && wl.letter != ' ') {
                active.push_back(wl.letter);
            }
        }
        return active;
    }

    bool isSolved() const
    {
        for (const auto& wl : m_word) {
            if (wl.state != WordLetterState::SOLVED && wl.letter != ' ') {
                return false;
            }
        }
        return true;
    }
private:
    WordArray m_word;
    WordGameStrategy m_strategy;

    std::string getString()
    {
        std::string text;
        text.reserve(m_word.size());
        for (auto& letterComp : m_word)
        {
            text.push_back(letterComp.letter);
        }
        return text;
    }

    RVector2 getPivotOffsets() {
        RVector2 size = MeasureTextEx(GetFontDefault(), getString().c_str(), fontSize, spacing);
        return { size.x * origin.x, fontSize * origin.y };
    }

    float measureChar(char c) {
        char buf[2] = { c, '\0' };
        return MeasureTextEx(GetFontDefault(), buf, fontSize, spacing).x;
    }

    void drawLetter(int i, const RVector2& pos)
    {
        WordLetter& letterComp = m_word[i];
        if (letterComp.style == WordLetterStyle::INVISIBLE)
            return;

        char drawingLetter = letterComp.letter;
        RColor col = {0, 0, 0, 255};

        switch (letterComp.state)
        {
        case WordLetterState::UNKNOWN:
            drawingLetter = '-';
            col = ::Color{0, 50, 100, 255};
            if (letterComp.style == WordLetterStyle::COLLIDEABLE)
                col.a = 180;
            else
                col.a = 70;
            break;
        case WordLetterState::KNOWN:
            col = ::Color{0, 92, 150, 255};
            if (letterComp.style == WordLetterStyle::COLLIDEABLE)
                col.a = 180;
            else
                col.a = 70;
            break;
        case WordLetterState::SOLVED:
            col = ::Color{0, 0, 0, 255};
            break;
        default:
            break;
        }

        char buf[2] = { drawingLetter, '\0' };
        ::DrawTextPro(font, buf, pos, {0, 0}, rotation, fontSize, spacing, col);
    }
};