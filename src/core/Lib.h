#pragma once

#include <cmath>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Color.hpp>

#include "VectorLib.h"

#define GET_VARIABLE_NAME(Variable) (#Variable)

template <typename T = float>
static constexpr T PI = (T)3.141592653589793238462643383279;

class Lib
{
public:
    template <typename T, typename U>
    static sf::Vector2<T> ConvertTo(const sf::Vector2<U> &vec) noexcept;

    template <typename T, typename U>
    static sf::Rect<T> ConvertTo(const sf::Rect<U> &rect) noexcept;

    template <typename T>
    static sf::Vector2<T> Mid(sf::Rect<T> rect) noexcept;

    static sf::Vector2f Mid(const sf::ConvexShape &polygon) noexcept;

    template <typename T>
    static sf::Vector2<T> MapPoint(const sf::Vector2<T> &point, sf::Rect<T> from, sf::Rect<T> to) noexcept;

    template <typename T>
    static T Constrain(const T &value, T from, T to);

    template <typename T>
    static T ToDegress(const T &radians);

    template <typename T>
    static T ToRadians(const T &degress);

    template <typename T>
    static sf::Color ValueToSpectrum(T value, T maxValue);

    static sf::ConvexShape CreateConvexShape(const std::vector<sf::Vector2f> &points);

    template <typename T>
    static std::vector<sf::Vector2<T>> WrapPoints(const std::vector<sf::Vector2<T>> &points);

private:
    template <typename T>
    static void ClearPointsRecursively(const std::pair<sf::Vector2<T>, sf::Vector2<T>> &line, const std::vector<sf::Vector2<T>> &points, std::vector<sf::Vector2<T>> &finalPoints);
};

template <typename T, typename U>
sf::Vector2<T> Lib::ConvertTo(const sf::Vector2<U> &vec) noexcept
{
    return sf::Vector2<T>((T)vec.x, (T)vec.y);
}

template <typename T, typename U>
sf::Rect<T> Lib::ConvertTo(const sf::Rect<U> &rect) noexcept
{
    return sf::Rect<T>((T)rect.left, (T)rect.top, (T)rect.width, (T)rect.height);
}

template <typename T>
sf::Vector2<T> Lib::Mid(sf::Rect<T> rect) noexcept
{
    return sf::Vector2<T>(rect.left + rect.width / (T)2, rect.top + rect.height / (T)2);
}

template <typename T>
sf::Vector2<T> Lib::MapPoint(const sf::Vector2<T> &point, sf::Rect<T> from, sf::Rect<T> to) noexcept
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
T Lib::Constrain(const T &value, T from, T to)
{
    if (value < from)
        return from;
    else if (value > to)
        return to;
    else
        return value;
}

template <typename T>
T Lib::ToDegress(const T &radians)
{
    return (radians * (T)180) / (T)PI<>;
}

template <typename T>
T Lib::ToRadians(const T &degress)
{
    return (T)(degress * PI<>) / (T)180;
}

template <typename T>
sf::Color Lib::ValueToSpectrum(T value, T maxValue)
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

template <typename T>
std::vector<sf::Vector2<T>> Lib::WrapPoints(const std::vector<sf::Vector2<T>> &points)
{
    std::vector<sf::Vector2f> finalPoints;

    std::vector<sf::Vector2f> topPoints;
    std::vector<sf::Vector2f> bottomPoints;
    std::pair<sf::Vector2f, sf::Vector2f> startLine;

    sf::Vector2f biggestX = {static_cast<T>(-std::numeric_limits<T>::infinity()), -std::numeric_limits<T>::infinity()};
    sf::Vector2f smallestX = {static_cast<T>(std::numeric_limits<T>::infinity()), std::numeric_limits<T>::infinity()};
    for (auto &point : points)
    {
        if (point.x > biggestX.x)
        {
            biggestX = point;
        }
        if (point.x < smallestX.x)
        {
            smallestX = point;
        }
    }
    startLine = std::make_pair(smallestX, biggestX);

    //Inital removal of center-points
    for (auto &point : points)
    {
        if (!vl::IsLeft(startLine.first, startLine.second, point))
        {
            topPoints.emplace_back(point);
        }
        else
        {
            bottomPoints.emplace_back(point);
        }
    }

    Lib::ClearPointsRecursively(startLine, topPoints, finalPoints);
    Lib::ClearPointsRecursively(std::make_pair(startLine.second, startLine.first), bottomPoints, finalPoints);

    return finalPoints;
}

template <typename T>
void Lib::ClearPointsRecursively(const std::pair<sf::Vector2<T>, sf::Vector2<T>> &line, const std::vector<sf::Vector2<T>> &points, std::vector<sf::Vector2<T>> &finalPoints)
{
    //Find the point which is the furthest away
    float biggestDistance = 0.0f;
    int biggestIndex = -1;
    for (size_t i = 0; i < points.size(); i++)
    {
        float currentCheck = vl::DistanceFromLine(line.first, line.second, points[i]);
        if (currentCheck > biggestDistance)
        {
            biggestDistance = currentCheck;
            biggestIndex = i;
        }
    }

    auto lineCpy = line;
    //Continues only if it can find a new point
    if (biggestIndex != -1)
    {
        sf::Vector2f furthest = points[biggestIndex];
        std::pair<sf::Vector2f, sf::Vector2f> newLine = {lineCpy.first, furthest};
        lineCpy.first = furthest;

        std::vector<sf::Vector2f> consideredPoints1;
        std::vector<sf::Vector2f> consideredPoints2;
        for (auto &point : points)
        {
            if (!vl::IsLeft(newLine.first, newLine.second, point))
            {
                consideredPoints1.push_back(point);
            }
            else if (!vl::IsLeft(lineCpy.first, lineCpy.second, point))
            {
                consideredPoints2.push_back(point);
            }
        }

        ClearPointsRecursively(newLine, consideredPoints1, finalPoints);
        ClearPointsRecursively(lineCpy, consideredPoints2, finalPoints);
    }
    else
    {
        finalPoints.push_back(lineCpy.first);
        finalPoints.push_back(lineCpy.second);
    }
}