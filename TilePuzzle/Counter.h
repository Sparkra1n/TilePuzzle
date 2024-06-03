#pragma once

#include <chrono>
#include <algorithm>

class Counter
{
public:
    using clock = std::chrono::steady_clock;

    Counter(const unsigned int targetFps) : 
        m_targetDeltaTime(1.0 / targetFps), 
        m_lastTime(clock::now())
    {}

    void update()
    {
        ++m_frameCount;
        const auto currentTime = clock::now();
        const std::chrono::duration<double> elapsed = currentTime - m_lastTime;

        // Apply a simple low-pass filter to smooth deltaTime
        constexpr double alpha = 0; // Smoothing factor between 0 and 1
        const double rawDeltaTime = elapsed.count();
        m_deltaTime = alpha * rawDeltaTime + (1.0 - alpha) * m_deltaTime;

        if (elapsed >= std::chrono::seconds(1))
        {
            m_fps = m_frameCount;
            m_frameCount = 0;
            m_lastTime = currentTime;
        }
    }

    [[nodiscard]] uint32_t getFps() const { return m_fps; }
    [[nodiscard]] double getDeltaTime() const { return std::min(m_targetDeltaTime, m_deltaTime); }

private:
    double m_targetDeltaTime;
    uint32_t m_frameCount{};
    clock::time_point m_lastTime;
    uint32_t m_fps{};
    double m_deltaTime{};
};

