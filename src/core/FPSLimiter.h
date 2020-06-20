#pragma once

#include <chrono>

#include <SFML/System/Sleep.hpp>

#include "Clock.h"

class FPSLimiter
{
public:
    static void Start() noexcept;
    static void Adjust();

    static float GetDesiredFPS() noexcept { return m_desiredFPS; }
    static void SetDesiredFPS(float fps) noexcept { m_desiredFPS = fps; }

private:
    static float m_desiredFPS;
    static std::chrono::steady_clock::time_point m_last;
};