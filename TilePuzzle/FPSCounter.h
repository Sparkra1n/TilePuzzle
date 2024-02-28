#pragma once

#include <SDL.h>
#include <functional>
#include <utility>

class FPSCounter
{
public:
	/**
	 * \brief 
	 * \tparam Args std::pair<void(func*)(double), double)
	 * \param targetFPS 
	 * \param args 
	 */
	//template<typename... Args>
 //   FPSCounter(const unsigned int targetFPS, Args&&... args)
 //       : m_targetDeltaTime(1.0 / targetFPS)
	//{
 //       (m_updateFunctions.emplace_back(std::make_pair(std::get<0>(args), std::get<1>(args))), ...);
 //   }

    FPSCounter(const unsigned int targetFPS) : m_targetDeltaTime(1.0 / targetFPS) {}


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

        m_deltaTime = static_cast<double>(elapsedTicks) / 1000.0;

        //// Call each update function with its corresponding time interval
        //for (const auto& [updateFunction, timeInterval] : m_updateFunctions)
        //{
        //    std::cout << m_deltaTime << "\r";
        //    if (m_deltaTime >= timeInterval)
        //    {
        //        updateFunction(m_deltaTime);
        //    }
        //}
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
    void callUpdateFunction(const std::function<void(double)>& updateFunction, const double timeInterval) const
    {
        if (m_deltaTime >= timeInterval) 
        {
            updateFunction(m_deltaTime);
        }
    }

    double m_targetDeltaTime;
    //std::vector<std::pair<std::function<void(double)>, double>> m_updateFunctions;
    uint32_t m_frameCount{};
    uint32_t m_lastTicks{};
    double m_fps{};
    double m_deltaTime{};
};

