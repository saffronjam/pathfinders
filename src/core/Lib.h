#pragma once

#include <cmath>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>

#define GET_VARIABLE_NAME(Variable) (#Variable)

template <typename T = float>
static constexpr T PI = (T)3.141592653589793238462643383279;

class Lib
{
public:
    template <typename T, typename U>
    static sf::Vector2<T> ConvertTo(const sf::Vector2<U> &vec) noexcept
    {
        return sf::Vector2<T>((T)vec.x, (T)vec.y);
    }
    template <typename T, typename U>
    static sf::Rect<T> ConvertTo(const sf::Rect<U> &rect) noexcept
    {
        return sf::Rect<T>((T)rect.left, (T)rect.top, (T)rect.width, (T)rect.height);
    }
    template <typename T>
    static sf::Vector2<T> Mid(sf::Rect<T> rect) noexcept
    {
        return sf::Vector2<T>(rect.left + rect.width / (T)2, rect.top + rect.height / (T)2);
    }

    template <typename T>
    static sf::Vector2<T> MapPoint(const sf::Vector2<T> &point, sf::Rect<T> from, sf::Rect<T> to) noexcept
    {
        sf::Vector2<T> _point = point;
        float x_diff = _point.x - from.left;
        float y_diff = _point.y - from.top;

        float x_percent_diff = x_diff / from.width;
        float y_percent_diff = y_diff / from.height;

        _point.x = to.left + to.width * x_percent_diff;
        _point.y = to.top + to.height * y_percent_diff;

        return _point;
    }

    template <typename T>
    static T Constrain(const T &value, T from, T to)
    {
        if (value < from)
            return from;
        else if (value > to)
            return to;
        else
            return value;
    }

    template <typename T>
    static T ToDegress(const T &radians)
    {
        return (radians * (T)180) / (T)PI<>;
    }
    template <typename T>
    static T ToRadians(const T &degress)
    {
        return (T)(degress * PI<>) / (T)180;
    }

    template <typename T>
    static sf::Color ValueToSpectrum(T value, T maxValue)
    {
        double a = ((double)value / (double)maxValue) / 0.2f;
        sf::Uint8 X = std::floor(a);
        sf::Uint8 Y = std::floor(255 * (a - X));
        switch (X)
        {
        case 0:
            return sf::Color(255, Y, 0);
        case 1:
            return sf::Color(255 - Y, 255, 0);
        case 2:
            return sf::Color(0, 255, Y);
        case 3:
            return sf::Color(0, 255 - Y, 255);
        case 4:
            return sf::Color(Y, 0, 255);
        case 5:
            return sf::Color(255, 0, 255);
        default:
            break;
        }
    };
};