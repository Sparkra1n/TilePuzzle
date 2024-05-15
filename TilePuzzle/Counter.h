#pragma once

#include <SDL.h>
#include <functional>
#include <utility>

class Counter
{
public:
    Counter(const unsigned int targetFPS) : m_targetDeltaTime(1.0 / targetFPS) {}


    void update()
    {
        ++m_frameCount;

        const uint32_t currentTicks = SDL_GetTicks();
        const uint32_t elapsedTicks = currentTicks - m_lastTicks;

        m_deltaTime = static_cast<double>(elapsedTicks) / 1000.0;  // Convert elapsed ticks to seconds

        if (elapsedTicks >= 1000)
        {
            m_fps = static_cast<double>(m_frameCount);  // Calculate FPS based on frames in last second
            m_frameCount = 0;
            m_lastTicks = currentTicks;
        }
    }


    [[nodiscard]] double getFPS() const
    {
        return m_fps;
    }

    [[nodiscard]] double getDeltaTime() const
    {
        return std::min(m_targetDeltaTime, m_deltaTime);
    }

private:
    //void callUpdateFunction(const std::function<void(double)>& updateFunction, const double timeInterval) const
    //{
    //    if (m_deltaTime >= timeInterval) 
    //    {
    //        updateFunction(m_deltaTime);
    //    }
    //}

    double m_targetDeltaTime;
    uint32_t m_frameCount{};
    uint32_t m_lastTicks{};
    double m_fps{};
    double m_deltaTime{};
};

