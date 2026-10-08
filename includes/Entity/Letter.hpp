#pragma once


#include "raylib.h"
#include <raylib-cpp.hpp>
#include "RectangePro.hpp"


class Letter
{
public:
    Color color;
    RVector2 pos;
    RVector2 origin = {0.5, 0.5};
    float rotation;

    Letter(char letter, float fontSize, const RVector2& pos, const Color& col = BLACK, float rotation = 0.f)
        : m_letter(letter),
        m_fontSize(fontSize),
        pos(pos),
        rotation(rotation),
        color(col)
    {
        m_updateLetterSize();
    }

    void setFontSize(float fontsize)
    {
        m_fontSize = fontsize;
        m_updateLetterSize();
    }

    float getFontSize()
    {
        return m_fontSize;
    }

    void setLetter(char letter)
    {
        m_letter = letter;
        m_updateLetterSize();
    }

    char getLetter()
    {
        return m_letter;
    }

    float getCollisionRadius() const
    {
        return m_fontSize / 5;
    }

    RVector2 getCollisionPos() const
    {
        RVector2 textOrigin = {
            m_textSize.x * origin.x, 
            m_textSize.y * origin.y
        };

        RVector2 centerPos = pos + (m_textSize / 2 - textOrigin).Rotate(rotation * DEG2RAD);
        return centerPos;
    }

    bool checkCollision(const RVector2& pos, float radius) const
    {
        return CheckCollisionCircles(getCollisionPos(), getCollisionRadius(), pos, radius);
    }

    bool checkCollision(const Letter& other) const
    {
        return checkCollision(other.getCollisionPos(), other.getCollisionRadius());
    }

    void draw()
    {
#ifdef DEBUG
        RVector2 centerPos = getCollisionPos();
        RColor bgcolor = {255, 0, 0, 100};
        DrawCircle(pos.x, pos.y, m_fontSize / 7, bgcolor);
        DrawCircle(centerPos.x, centerPos.y, m_fontSize / 7, {0, 0, 255, 100});
#endif
        const char* text = TextFormat("%c", m_letter);
        RFont font = GetFontDefault();

        RVector2 textOrigin = {
            m_textSize.x * origin.x, 
            m_textSize.y * origin.y
        };

        ::DrawTextPro(
            font, 
            text,
            pos,
            textOrigin,
            rotation,
            m_fontSize,
            0.f, 
            color
        );
    }

protected:
    RVector2 m_textSize;
    char m_letter;
    float m_fontSize;

    void m_updateLetterSize()
    {
        RFont font = GetFontDefault();
        m_textSize = MeasureTextEx(font, TextFormat("%c", m_letter), m_fontSize, 0.f);
    }
};

