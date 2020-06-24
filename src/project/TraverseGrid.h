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

    std::map<long, Node> &GetNodes() noexcept { return GetActiveNodes(); }
    long GetNodeUID(const sf::Vector2f &position) const;
    long GetNodeUID(const sf::Vector2f &position, Type type) const;
    long GetStartUID() const noexcept { return m_startUID; }
    long GetGoalUID() const noexcept { return m_goalUID; }
    std::vector<long> &GetSubGoalUIDs() noexcept { return m_subGoalUIDs; }

    bool IsObstacle(long uid) const { return m_obstacleUIDs.find(uid) != m_obstacleUIDs.end(); }
    bool IsStart(long uid) const noexcept { return m_startUID == uid; }
    bool IsGoal(long uid) const noexcept { return m_goalUID == uid; }
    bool IsSubGoal(long uid) const noexcept { return std::find(m_subGoalUIDs.begin(), m_subGoalUIDs.end(), uid) != m_subGoalUIDs.end(); }
    bool IsClear(long uid) const noexcept;

    void SetStart(const sf::Vector2f &position);
    void SetStart(long uid);
    void SetGoal(const sf::Vector2f &position);
    void SetGoal(long uid);
    void ResetStartGoal();

    void AddSubGoal(const sf::Vector2f &position);
    void AddSubGoal(long uid);
    void RemoveSubGoal(const sf::Vector2f &position);
    void RemoveSubGoal(long uid);
    void ClearSubGoals();

    void AddObstacle(const sf::Vector2f &position);
    void AddObstacle(long uid);
    void RemoveObstacle(const sf::Vector2f &position);
    void RemoveObstacle(long uid);
    void ClearObstacles();

private:
    void DrawSquareGrid();
    void DrawVoronoiGrid();

    void GenerateGrids();
    void CalculateSquareGridNeighbors();
    void CalculateVoronoiGridNeighbors();

    void ClearNodeColor(long uid);
    void SetNodeColor(long uid, const sf::Color &color);

    std::map<long, Node> &GetActiveNodes() noexcept;
    const std::map<long, Node> &GetActiveNodesConst() const noexcept;

private:
    TraverseGrid::Type m_currentType;

    std::map<long, Node> m_squareGridNodes;
    std::map<long, Node> m_voronoiGridNodes;
    std::set<long> m_obstacleUIDs;

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
    std::vector<long> m_subGoalUIDs;
};