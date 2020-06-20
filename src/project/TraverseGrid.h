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

    const Node *GetStartNode() const { return &m_nodes.at(GetStartNodeUID()); };
    const Node *GetGoalNode() const { return &m_nodes.at(GetGoalNodeUID()); };
    long GetStartNodeUID() const noexcept { return m_startUID; }
    long GetGoalNodeUID() const noexcept { return m_goalUID; }

    void SetStart(const sf::Vector2f &start);
    void SetGoal(const sf::Vector2f &goal);

    std::map<long, Node> &GetNodes() noexcept { return m_nodes; }

private:
    void DrawSquareGrid();
    void DrawVoronoiGrid();

    void GenerateGrid();
    void CalculateNeighbors();

private:
    TraverseGrid::Type m_currentType;

    long m_startUID;
    long m_goalUID;
    std::map<long, Node> m_nodes;

    sf::FloatRect m_visRect;
    sf::Vector2i m_nBoxes;
};