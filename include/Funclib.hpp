#pragma once

#include <vector>
#include <array>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Image.hpp>

class Funclib
{
public:
    static std::vector<sf::Vector2f> WrapPoints(std::vector<sf::Vector2f> *points);
    static void MapPointToRect(sf::Vector2f &point, sf::FloatRect rect);
    static void TranslatePointFromRectToRect(sf::Vector2f &point, sf::FloatRect from, sf::FloatRect to);

    //voronoi
    static void ApplyLloydsRelaxtion(std::vector<class VEdge> &edges, std::vector<class VoronoiPoint *> &ver, std::vector<sf::ConvexShape> &voronoi_shapes, double minY, double maxY, float k);
    static std::vector<sf::ConvexShape> CreateShapeListFromVonoroi(std::vector<class VoronoiPoint *> &ver, std::vector<class VEdge> &edges);

private:
    static void ClearPointsRecursively(std::pair<sf::Vector2f, sf::Vector2f> line, std::vector<sf::Vector2f *> *points, std::vector<sf::Vector2f> *finalPoints);
};

class vf
{
public:
    static sf::Vector2f LineMiddlePoint(sf::Vector2f point1, sf::Vector2f point2);
    static sf::Vector2f Direction(sf::Vector2f point1, sf::Vector2f point2);
    static sf::Vector2f Unit(sf::Vector2f vector);
    static sf::Vector2f Null();
    static sf::Vector2f Perpendicular(sf::Vector2f vector);
    static sf::Vector2f InDefIntersection(std::pair<sf::Vector2f, sf::Vector2f> line1, std::pair<sf::Vector2f, sf::Vector2f> line2);
    static float Length(sf::Vector2f vector);
    static float LengthSq(sf::Vector2f vector);
    static float Distance(sf::Vector2f const &u, sf::Vector2f const &v);
    static void Normalize(sf::Vector2f &vector);
    static float Angle(sf::Vector2f const &v1, sf::Vector2f const &v2);
    static float Slope(sf::Vector2f point1, sf::Vector2f point2);
    static bool isLeft(sf::Vector2f line_point1, sf::Vector2f line_point2, sf::Vector2f point);
    static float DistanceFromLine(sf::Vector2f line_point1, sf::Vector2f line_point2, sf::Vector2f point);
    static float Determinant(sf::Vector2f const &u, sf::Vector2f const &v);
    static float DotProduct(sf::Vector2f const &u, sf::Vector2f const &v);
};

class gf
{
public:
    static bool IsInBetween(float const &value, float const &lower_bound, float const &upper_bound);
    static int Constrain(int const &x, int const &lower, int const &upper);
    static float Constrain(float const &x, float const &lower, float const &upper);
    static float ConstrainLower(float const &x, float const &lower);
    static float ConstrainUpper(float const &x, float const &upper);
    static float Map(float const &x, float const &lower_from, float const &upper_from, float const &lower_to, float const &upper_to);
    static void Interpolation(float &x, float const &y, float const &time, float const &deltaTime);
    static void AngleInterpolation(float &x, float y, float const &time, float const &dt);
    static int Quo(int a, int b);
};

class sfmlext
{
public:
    static bool UniqueInConvexShape(sf::ConvexShape &convexShape, sf::Vector2f &point);
    static sf::ConvexShape CreateConvexShapeFromPointList(std::vector<sf::Vector2f> unsorted_list);
    static sf::Vector2f GetCentroidOfPolygon(sf::ConvexShape polygon);
    static sf::Vector2f GetCentroidOfRectangle(sf::FloatRect polygon);
    static sf::Rect<float> RectFromCenter(sf::Vector2f mid, float half_width, float half_height);
    static sf::Vector2f ClosestPolygonVertex(sf::ConvexShape polygon, sf::Vector2f point);
    static sf::Color ColorGradient(sf::Image &colors, float x);
    static bool PolygonContains(sf::ConvexShape const &polygon, sf::Vector2f const &point);

private:
    static std::vector<sf::Vector2f> SortPolygonVerticies(sf::ConvexShape const &polygon);
};