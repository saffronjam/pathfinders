#include "Lib.h"

sf::Vector2f Lib::Mid(const sf::ConvexShape &polygon) noexcept
{
    size_t nPoints = polygon.getPointCount();
    sf::Vector2f total(0.0f, 0.0f);
    for (size_t i = 0; i < nPoints; i++)
        total += polygon.getPoint(i);
    return total / static_cast<float>(nPoints);
}

sf::ConvexShape Lib::CreateConvexShape(const std::vector<sf::Vector2f> &points)
{
    std::vector<sf::Vector2f> usablePoints = Lib::WrapPoints(points);

    sf::ConvexShape finalShape;
    finalShape.setPointCount(usablePoints.size() / 2);

    for (size_t i = 0, currentPoint = 0; i < usablePoints.size() - 1; i += 2, currentPoint++)
    {
        finalShape.setPoint(currentPoint, usablePoints[i]);
    }

    return finalShape;
}