#pragma once

#include "Node.h"
#include "Camera.h"
#include "Voronoi.h"

class TraverseGrid
{
public:
    enum class Type
    {
        Square,
        Voronoi
    };

public:
    TraverseGrid(TraverseGrid::Type type, const sf::FloatRect &visRect);

    void Draw();

    void ChangeGridType(TraverseGrid::Type type);
    void CalculateNeighbors(std::map<long, Node> &nodes) const;

    std::map<long, Node> &GetNodes() noexcept { return m_nodes; }
    long GetNodeUIDByPosition(const sf::Vector2f &position) const;
    long GetStartUID() const noexcept { return m_startUID; }
    long GetGoalUID() const noexcept { return m_goalUID; }
    bool IsObstacle(long uid) const { return m_obstacles.find(uid) != m_obstacles.end(); }
    bool IsStart(long uid) const noexcept { return m_startUID == uid; }
    bool IsGoal(long uid) const noexcept { return m_goalUID == uid; }

    void SetStart(const sf::Vector2f &start);
    void SetStart(long uid);
    void SetGoal(const sf::Vector2f &goal);
    void SetGoal(long uid);

    void SetIsObstacle(long uid, bool isObstacle);
    void ClearObstacles();

private:
    void DrawSquareGrid();
    void DrawVoronoiGrid();

    void GenerateGrid();

    void ClearNodeColor(long uid);
    void SetNodeColor(long uid, const sf::Color &color);

private:
    TraverseGrid::Type m_currentType;

    std::map<long, Node> m_nodes;
    std::set<long> m_obstacles;

    sf::FloatRect m_visRect;
    sf::Vector2i m_nBoxes;

    std::map<long, sf::RectangleShape> m_squareGrid;
    Voronoi m_voronoiGrid;

    sf::Color m_obstacleColor;
    sf::Color m_startColor;
    sf::Color m_goalColor;

    // Cached
    long m_startUID;
    long m_goalUID;
};