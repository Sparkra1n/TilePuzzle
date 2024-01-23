#pragma once

#include <SDL.h>

class FPSCounter
{
public:
    FPSCounter() : m_frameCount(0), m_lastTicks(0), m_fps(0) {}

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
    }

    [[nodiscard]] double getFPS() const
	{
        return m_fps;
    }

private:
    int m_frameCount;
    uint32_t m_lastTicks;
    double m_fps;
};
