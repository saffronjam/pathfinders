#include "Lib.h"

sf::Vector2f Lib::Mid(const sf::ConvexShape &polygon) noexcept
{
    size_t nPoints = polygon.getPointCount();
    sf::Vector2f total(0.0f, 0.0f);
    for (size_t i = 0; i < nPoints; i++)
        total += polygon.getPoint(i);
    return total / static_cast<float>(nPoints);
}