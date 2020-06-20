#pragma once

#include <utility>
#include <random>

#include <SFML/Graphics/Color.hpp>

class Random
{
public:
    Random(int const &lower = 0, int const &upper = 10)
        : m_lower(lower),
          m_upper(upper){};

    int Gen() { return (double)((rand() % ((int)m_upper - (int)m_lower)) + (int)m_lower); }

    void SetLowerBound(int lowerBound_IN) { m_lower = lowerBound_IN; };
    void SetUpperBound(int upperBound_IN) { m_upper = upperBound_IN; };

    static int Integer(int low = 0, int high = 100)
    {
        static std::random_device rd;
        static std::mt19937 e(rd());
        std::uniform_real_distribution<float> dis(static_cast<float>(low), static_cast<float>(high));
        return static_cast<int>(dis(e));
    }

    static float Float(float low = 0.0f, float high = 1.0f)
    {
        static std::random_device rd;
        static std::mt19937 e(rd());
        std::uniform_real_distribution<float> dis(low, high);
        return dis(e);
    }

    static sf::Color Color(bool randomizeAlpha = false)
    {
        sf::Uint8 r = Random::Integer(0, 255);
        sf::Uint8 g = Random::Integer(0, 255);
        sf::Uint8 b = Random::Integer(0, 255);
        sf::Uint8 a = 255;
        if (randomizeAlpha)
            a = Random::Integer(0, 255);
        return sf::Color(r, g, b, a);
    }

    template <typename T>
    static sf::Vector2<T> Vec2(T lowX, T lowY, T highX, T highY)
    {
        float x = Random::Float(lowX, highX);
        float y = Random::Float(lowY, highY);
        return sf::Vector2<T>((T)x, (T)y);
    }

private:
    int m_lower;
    int m_upper;
};