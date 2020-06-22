#pragma once

#include "Node.h"
#include "Camera.h"

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

private:
    void DrawSquareGrid();
    void DrawVoronoiGrid();

    void GenerateGrid();

private:
    TraverseGrid::Type m_currentType;

    std::map<long, Node> m_nodes;
    std::set<long> m_obstacles;

    sf::FloatRect m_visRect;
    sf::Vector2i m_nBoxes;

    // Cached
    long m_startUID;
    long m_goalUID;
};