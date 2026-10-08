#pragma once

#include "Letter.hpp"
#include <cmath>

constexpr float CURVE_ROTATION = 45.f;

enum class MoveType
{
    LINEAR,     // Straight line
    SINE_WAVE,  // Smooth oscillation
    CURVE       // Sharp zigzag: +45, then -45 repeatedly
};

class MoveLetter : public Letter
{
public:
    RVector2 velocity = {0, 0};
    float rotVelocity = 0;

    // Updated constructor to initialize MoveType
    MoveLetter(char letter, float fontSize, const RVector2& pos, MoveType type, const Color& col = BLACK, float rotation = 0.f)
        : Letter(std::move(letter), std::move(fontSize), std::move(pos), std::move(col), std::move(rotation)),
          m_moveType(type)
    {
        
    }

    void setVelocity(const RVector2& vel) {
        velocity = vel;
        if (m_moveType == MoveType::CURVE)
        {
            velocity = velocity.Rotate(-CURVE_ROTATION / 2 * DEG2RAD);
        }
    }

    // Now uses the internal m_moveType instead of passing it every frame
    void updatePhysics()
    {
        float dt = GetFrameTime();
        m_timer += dt;

        switch (m_moveType)
        {
        case MoveType::LINEAR:
        {
            pos += velocity * dt;
            break;
        }

        case MoveType::SINE_WAVE:
        {
            pos += velocity * dt;
            RVector2 perp = {-velocity.y, velocity.x}; 
            float speed = velocity.Length();
            if (speed > 0) perp /= speed;

            float wave = cosf(m_timer * speed / 75.f) * speed;
            pos += perp * wave * dt;
            break;
        }

        case MoveType::CURVE:
        {
            pos += velocity * dt;

            if (m_timer > 0.4f)
            {
                m_timer = 0.0f;
                velocity = velocity.Rotate(45.0f * m_turnDirection * DEG2RAD);
                m_turnDirection *= -1.0f; 
            }
            break;
        }
        }

        rotation += rotVelocity * dt;
    }

private:
    MoveType m_moveType;
    float m_timer = 0.0f;
    float m_turnDirection = 1.0f; 
};