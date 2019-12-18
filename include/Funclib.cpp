#include "Funclib.hpp"

#include <cmath>

#include "Voronoi.hpp"

std::vector<sf::Vector2f> Funclib::WrapPoints(std::vector<sf::Vector2f> *points)
{
    std::vector<sf::Vector2f> finalPoints;

    std::vector<sf::Vector2f *> topPoints;
    std::vector<sf::Vector2f *> bottomPoints;
    std::pair<sf::Vector2f, sf::Vector2f> startLine;

    sf::Vector2f biggestX = {-10000, -10000};
    sf::Vector2f smallestX = {10000, 10000};
    for (auto &point : *points)
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
    startLine = {smallestX, biggestX};

    //Inital removal of center-points
    for (auto &point : *points)
    {
        if (!vf::isLeft(startLine.first, startLine.second, point))
        {
            topPoints.emplace_back(&point);
        }
        else
        {
            bottomPoints.emplace_back(&point);
        }
    }

    ClearPointsRecursively(startLine, &topPoints, &finalPoints);
    ClearPointsRecursively({startLine.second, startLine.first}, &bottomPoints, &finalPoints);

    return finalPoints;
}

void Funclib::ClearPointsRecursively(std::pair<sf::Vector2f, sf::Vector2f> line, std::vector<sf::Vector2f *> *points, std::vector<sf::Vector2f> *finalPoints)
{
    //Find the point which is the furthest away
    float biggestDistance = 0.0f;
    int biggestIndex = -1;
    for (size_t i = 0; i < points->size(); i++)
    {
        float currentCheck = vf::DistanceFromLine(line.first, line.second, *(*points)[i]);
        if (currentCheck > biggestDistance)
        {
            biggestDistance = currentCheck;
            biggestIndex = i;
        }
    }

    //Continues only if it can find a new point
    if (biggestIndex != -1)
    {
        sf::Vector2f furthest = *(*points)[biggestIndex];
        std::pair<sf::Vector2f, sf::Vector2f> newLine = {line.first, furthest};
        line.first = furthest;

        std::vector<sf::Vector2f *> consideredPoints1;
        std::vector<sf::Vector2f *> consideredPoints2;
        for (auto &point : *points)
        {
            if (!vf::isLeft(newLine.first, newLine.second, *point))
            {
                consideredPoints1.emplace_back(point);
            }
            else if (!vf::isLeft(line.first, line.second, *point))
            {
                consideredPoints2.emplace_back(point);
            }
        }

        ClearPointsRecursively(newLine, &consideredPoints1, finalPoints);
        ClearPointsRecursively(line, &consideredPoints2, finalPoints);
    }
    else
    {
        finalPoints->emplace_back(line.first.x, line.first.y);
        finalPoints->emplace_back(line.second.x, line.second.y);
    }
}

void Funclib::MapPointToRect(sf::Vector2f &point, sf::FloatRect rect)
{
    float &x = point.x;
    float &y = point.y;
    if (x < rect.left)
    {
        x = rect.left;
    }
    else if (x > rect.left + rect.width)
    {
        x = rect.left + rect.width;
    }
    if (y < rect.top)
    {
        y = rect.top;
    }
    else if (y > rect.top + rect.height)
    {
        y = rect.top + rect.height;
    }
}

void Funclib::TranslatePointFromRectToRect(sf::Vector2f &point, sf::FloatRect from, sf::FloatRect to)
{
    float &x = point.x;
    float &y = point.y;

    float x_diff = x - from.left;
    float y_diff = y - from.top;

    float x_percent_diff = x_diff / from.width;
    float y_percent_diff = y_diff / from.height;

    x = to.left + to.width * x_percent_diff;
    y = to.top + to.height * y_percent_diff;
}

void Funclib::ApplyLloydsRelaxtion(std::vector<VEdge> &edges, std::vector<VoronoiPoint *> &ver, std::vector<sf::ConvexShape> &voronoi_shapes, double minY, double maxY, float k)
{
    for (int i = 0; i < k; i++)
    {
        for (size_t i = 0; i < ver.size(); i++)
        {
            ver[i]->x = sfmlext::GetCentroidOfPolygon(voronoi_shapes[i]).x;
            ver[i]->y = sfmlext::GetCentroidOfPolygon(voronoi_shapes[i]).y;
        }
        Voronoi *vdg = new Voronoi();
        edges = vdg->ComputeVoronoiGraph(ver, minY, maxY);
        delete vdg;
        voronoi_shapes = Funclib::CreateShapeListFromVonoroi(ver, edges);
    }
}

std::vector<sf::ConvexShape> Funclib::CreateShapeListFromVonoroi(std::vector<VoronoiPoint *> &ver, std::vector<VEdge> &edges)
{
    std::vector<sf::ConvexShape> final;
    for (std::vector<VoronoiPoint *>::iterator i = ver.begin(); i != ver.end(); i++)
    {
        std::vector<sf::Vector2f> convexShapeVerticies;
        for (std::vector<VEdge>::iterator j = edges.begin(); j != edges.end(); j++)
        {
            if (((j->Left_Site.x == (*i)->x) && (j->Left_Site.y == (*i)->y)) || ((j->Right_Site.x == (*i)->x) && (j->Right_Site.y == (*i)->y)))
            {
                sf::Vector2f VertexA = sf::Vector2f(j->VertexA.x, j->VertexA.y);
                sf::Vector2f VertexB = sf::Vector2f(j->VertexB.x, j->VertexB.y);
                if (std::find(convexShapeVerticies.begin(), convexShapeVerticies.end(), VertexA) == convexShapeVerticies.end())
                {
                    convexShapeVerticies.push_back(VertexA);
                }
                if (std::find(convexShapeVerticies.begin(), convexShapeVerticies.end(), VertexB) == convexShapeVerticies.end())
                {
                    convexShapeVerticies.push_back(VertexB);
                }
            }
        }
        sf::ConvexShape myShape = sfmlext::CreateConvexShapeFromPointList(convexShapeVerticies);
        myShape.setOutlineColor(sf::Color::Blue);
        myShape.setOutlineThickness(1);
        final.push_back(myShape);
    }
    return final;
}

//Vector functions
sf::Vector2f vf::LineMiddlePoint(sf::Vector2f point1, sf::Vector2f point2)
{
    sf::Vector2f delta = point2 - point1;
    delta.x /= 2.0f;
    delta.y /= 2.0f;
    return point1 + delta;
}

sf::Vector2f vf::Direction(sf::Vector2f point1, sf::Vector2f point2)
{
    return vf::Unit(point2 - point1);
}

sf::Vector2f vf::Unit(sf::Vector2f vector)
{
    return vector / vf::Length(vector);
}

sf::Vector2f vf::Null()
{
    return sf::Vector2f(0, 0);
}

sf::Vector2f vf::Perpendicular(sf::Vector2f vector)
{
    return sf::Vector2f(-vector.y, vector.x);
}

sf::Vector2f vf::InDefIntersection(std::pair<sf::Vector2f, sf::Vector2f> line1, std::pair<sf::Vector2f, sf::Vector2f> line2)
{
    float line1_k = Slope(line1.first, line1.second);
    float line2_k = Slope(line2.first, line2.second);
    if (line1_k == line2_k)
    {
        //Parallell lines have none or unlimited intersections
        return sf::Vector2f(-1, -1);
    }

    float line1_m = line1.first.y - line1_k * line1.first.x;
    float line2_m = line2.first.y - line2_k * line2.first.x;

    sf::Vector2f intersection;
    intersection.x = (line2_m - line1_m) / (line1_k - line2_k);
    intersection.y = line1_k * intersection.x;

    return intersection;
}

float vf::Length(sf::Vector2f vector)
{
    return sqrt(vf::LengthSq(vector));
}

float vf::LengthSq(sf::Vector2f vector)
{
    return vector.x * vector.x + vector.y * vector.y;
}

float vf::Distance(sf::Vector2f const &u, sf::Vector2f const &v)
{
    return sqrt(pow(u.x - v.x, 2) + pow(u.y - v.y, 2));
}

void vf::Normalize(sf::Vector2f &vector)
{
    float length = vf::Length(vector);
    if (length != 0.0f)
    {
        vector.x /= length;
        vector.y /= length;
    }
}

float vf::Slope(sf::Vector2f point1, sf::Vector2f point2)
{
    if (point1.x > point2.x)
    {
        std::swap(point1, point2);
    }
    return (point2.y - point1.y) / (point2.x - point1.x);
}

bool vf::isLeft(sf::Vector2f line_point1, sf::Vector2f line_point2, sf::Vector2f point)
{
    return ((line_point2.x - line_point1.x) * (point.y - line_point1.y) - (line_point2.y - line_point1.y) * (point.x - line_point1.x)) > 0.0f;
}

float vf::DistanceFromLine(sf::Vector2f line_point1, sf::Vector2f line_point2, sf::Vector2f point)
{
    return abs(((line_point2.x - line_point1.x) * (point.y - line_point1.y) - (line_point2.y - line_point1.y) * (point.x - line_point1.x)) / vf::Length(line_point2 - line_point1));
}

bool gf::IsInBetween(float const &value, float const &lower_bound, float const &upper_bound)
{
    float _low = lower_bound;
    float _upper = upper_bound;
    if (lower_bound > upper_bound)
    {
        std::swap(_low, _upper);
    }
    return value >= _low && value <= _upper;
}

int gf::Constrain(int const &x, int const &lower, int const &upper)
{
    return (int)gf::Constrain(x, lower, upper);
}

float gf::Constrain(float const &x, float const &lower, float const &upper)
{
    float final = x;
    if (final < lower)
    {
        final = lower;
    }
    else if (final > upper)
    {
        final = upper;
    }
    return final;
}

float gf::ConstrainLower(float const &x, float const &lower)
{
    return gf::Constrain(x, lower, x);
}
float gf::ConstrainUpper(float const &x, float const &upper)
{
    return gf::Constrain(x, x, upper);
}

float gf::Map(float const &x, float const &lower_from, float const &upper_from, float const &lower_to, float const &upper_to)
{
    float _lowFrom = lower_from;
    float _upperFrom = upper_from;
    float _lowTo = lower_to;
    float _upperTo = upper_to;
    if (_upperFrom < _lowFrom)
    {
        std::swap(_lowFrom, _upperFrom);
    }
    if (_upperTo < _lowTo)
    {
        std::swap(_lowTo, _upperTo);
    }
    float difference_from = _upperFrom - _lowFrom;
    float difference_to = _upperTo - _lowTo;

    float difference_from_percent = (x - _lowFrom) / difference_from;

    return _lowTo + difference_to * difference_from_percent;
}

bool sfmlext::UniqueInConvexShape(sf::ConvexShape &convexShape, sf::Vector2f &point)
{
    bool unique = true;
    for (size_t i = 0; i < convexShape.getPointCount(); i++)
    {
        if ((int)convexShape.getPoint(i).x == (int)point.x && (int)convexShape.getPoint(i).y == (int)point.y)
        {
            unique = false;
        }
    }
    return unique;
}

void gf::Interpolation(float &x, float const &y, float const &time, float const &deltaTime)
{
    x += (2.f * (y - x) * deltaTime) / time;
}
void gf::AngleInterpolation(float &x, float y, float const &time, float const &dt)
{
    if (abs(y - x) > 180)
    {
        if (x > y)
            y += 360;
        else
            x += 360;
    }
    x += (2 * (y - x) * dt) / time;

    if (x > 360)
        x = (int)x % 360;
}

int gf::Quo(int a, int b)
{
    int q = 0;
    if (b == 0)
        return 0;
    else
        while (a - b >= 0)
        {
            a = a - b;
            q++;
        }
    return q;
}

sf::ConvexShape sfmlext::CreateConvexShapeFromPointList(std::vector<sf::Vector2f> points)
{
    std::vector<sf::Vector2f> usable_points = Funclib::WrapPoints(&points);

    sf::ConvexShape finalShape;
    finalShape.setPointCount(usable_points.size() / 2);

    for (size_t i = 0, currentPoint = 0; i < usable_points.size() - 1; i += 2, currentPoint++)
    {
        finalShape.setPoint(currentPoint, usable_points[i]);
    }

    return finalShape;
}

sf::Vector2f sfmlext::GetCentroidOfPolygon(sf::ConvexShape polygon)
{
    sf::Vector2f sum_of_verticies(0.0f, 0.0f);
    unsigned int number_of_verticies = polygon.getPointCount();

    for (unsigned int i = 0; i < number_of_verticies; i++)
    {
        sum_of_verticies += polygon.getPoint(i);
    }

    sum_of_verticies.x /= number_of_verticies;
    sum_of_verticies.y /= number_of_verticies;

    return sum_of_verticies;
}

sf::Vector2f sfmlext::GetCentroidOfRectangle(sf::FloatRect rectangle)
{
    float x = rectangle.left + rectangle.width / 2.0f;
    float y = rectangle.top + rectangle.height / 2.0f;
    return sf::Vector2f(x, y);
}

sf::Rect<float> sfmlext::RectFromCenter(sf::Vector2f mid, float half_width, float half_height)
{
    sf::Rect<float> final;
    final.left = mid.x - half_width;
    final.top = mid.y - half_height;
    final.width = half_width * 2.0f;
    final.height = half_height * 2.0f;
    return final;
}

sf::Vector2f sfmlext::ClosestPolygonVertex(sf::ConvexShape polygon, sf::Vector2f point)
{
    float smallest_dist_sq = -1;
    sf::Vector2f final(-1.0f, -1.0f);

    for (size_t i = 0; i < polygon.getPointCount(); i++)
    {
        float dist_sq = vf::LengthSq(sf::Vector2f(point - polygon.getPoint(i)));

        if (smallest_dist_sq == -1 || dist_sq < smallest_dist_sq)
        {
            smallest_dist_sq = dist_sq;
            final = polygon.getPoint(i);
        }
    }
    return final;
}

sf::Color sfmlext::ColorGradient(sf::Image &color, float x)
{
    if (x > 0.999)
        x = 0.999;
    if (x < 0.001)
        x = 0.001;
    return color.getPixel((int)(x * color.getSize().x), 0);
}

bool sfmlext::PolygonContains(sf::ConvexShape const &polygon, sf::Vector2f const &point)
{
    std::vector<sf::Vector2f> allVertices = SortPolygonVerticies(polygon);
    bool allPointsInPolygon = true;
    for (size_t i = 0; i < allVertices.size() - 1; i += 2)
    {
        if (!vf::isLeft(allVertices[i], allVertices[i + 1], point))
        {
            allPointsInPolygon = false;
            break;
        }
    }
    return allPointsInPolygon;
}

std::vector<sf::Vector2f> sfmlext::SortPolygonVerticies(sf::ConvexShape const &polygon)
{
    std::vector<sf::Vector2f> allVertices;
    for (size_t i = 0; i < polygon.getPointCount(); i++)
    {
        allVertices.push_back(polygon.getPoint(i));
    }
    return Funclib::WrapPoints(&allVertices);
}