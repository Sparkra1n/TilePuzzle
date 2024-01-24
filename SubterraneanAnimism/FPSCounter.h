#pragma once

#include <SDL.h>

class FPSCounter
{
public:
    FPSCounter() : m_frameCount(0), m_lastTicks(0), m_fps(0), m_deltaTime(0) {}

    void update()
    {
        ++m_frameCount;

        const uint32_t currentTicks = SDL_GetTicks();
        const uint32_t elapsedTicks = currentTicks - m_lastTicks;

        if (elapsedTicks >= 1000)
        {
            m_fps = static_cast<double>(m_frameCount) / (elapsedTicks / 1000.0);
            m_frameCount = 0;
            m_lastTicks = currentTicks;
        }

        constexpr double targetFPS = 60.0;
        constexpr double targetDeltaTime = 1.0 / targetFPS;
        m_deltaTime = targetDeltaTime;
    }

    [[nodiscard]] double getFPS() const
    {
        return m_fps;
    }

    [[nodiscard]] double getDeltaTime() const
    {
        return m_deltaTime;
    }

private:
    uint32_t m_frameCount;
    uint32_t m_lastTicks;
    double m_fps;
    double m_deltaTime;
};
