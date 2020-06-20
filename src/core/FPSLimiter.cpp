#include "FPSLimiter.h"

#include "Log.h"

using namespace std::chrono;

float FPSLimiter::m_desiredFPS = 0.0f;
steady_clock::time_point FPSLimiter::m_last;

void FPSLimiter::Start() noexcept
{
    m_last = steady_clock::now();
}

void FPSLimiter::Adjust()
{
    if (m_desiredFPS != 0.0f)
    {
        const auto old = m_last;
        m_last = steady_clock::now();

        const duration<float> chronoFrameTime = m_last - old;
        float frameTime = chronoFrameTime.count();
        float secondsRemaining = (1.0f / m_desiredFPS) - frameTime;

        sf::sleep(sf::seconds(secondsRemaining));
    }
}