#pragma once

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

class Clock
{
public:
    static const sf::Time &Delta() noexcept { return m_dt; }
    static float GetFPS() noexcept { return m_dt.asSeconds() > 0.0f ? 1.0f / m_dt.asSeconds() : 0.0f; }

    static void Mark() noexcept { m_dt = m_clock.getElapsedTime(); }
    static void Reset() noexcept { m_clock.restart(); };

private:
    static sf::Clock m_clock;
    static sf::Time m_dt;
};
