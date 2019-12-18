#pragma once

#include <memory>

#include "Graphics.hpp"
#include "Node.hpp"
#include "InputUtility.hpp"
#include "Voronoi.hpp"

class Grid
{
public:
    enum GridMode : unsigned char
    {
        ModeBox,
        ModePolygon
    };

private:
    class TraverseBase
    {
    public:
        TraverseBase(sf::Vector2i const &nodePosition);

        void SetIsBlocked(bool const &isBlocked) { m_isBlocked = isBlocked; }

        sf::Vector2i GetPosition() { return m_node->GetPosition(); }
        std::vector<std::shared_ptr<Node>> GetNeighbors() { return m_node->GetNeighbors(); }
        std::shared_ptr<Node> GetNode() { return m_node; }
        bool GetIsBlocked() { return m_isBlocked; }

    protected:
        std::shared_ptr<Node> m_node;
        bool m_isBlocked;
    };

    class TraverseBox : public TraverseBase
    {
    public:
        TraverseBox(sf::IntRect const &rectangle);
        void SetColor(sf::Color color) { m_rectangleShape.setFillColor(color); }

        sf::RectangleShape GetRectangleShape() const { return m_rectangleShape; }

    private:
        sf::RectangleShape m_rectangleShape;
    };

    class TraversePolygon : public TraverseBase
    {
    public:
        TraversePolygon(sf::ConvexShape const &polygon);
        void SetColor(sf::Color color) { m_polygon.setFillColor(color); }

        sf::ConvexShape GetPolygon() const { return m_polygon; }

    private:
        sf::ConvexShape m_polygon;
    };

public:
    Grid(InputUtility &iu, class AStar &solver, sf::Vector2i const &boxSize);

    void Update();

    void DrawGrid(Graphics &gfx);
    void DrawInBoxGrid(Graphics &gfx, sf::Vector2i coord);

    void ChangeToBoxMode();
    void ChangeToPolygonMode();

    TraverseBox GetTraverseBox(sf::Vector2i coord) { return *m_boxGridLayout[coord.x % m_size.x + coord.y * m_size.x]; }
    std::vector<std::shared_ptr<TraverseBox>> GetAllTraverseBoxes() { return m_boxGridLayout; }
    std::vector<std::shared_ptr<TraversePolygon>> GetAllTraversePolygons() { return m_polygonGridLayout; }

    bool *GetIsPlacingObstaclesPtr() { return &m_isPlacingObstacles; }
    bool *GetIsRemovingObstaclesPtr() { return &m_isRemovingObstacles; }
    bool *GetIsPlacingStartPtr() { return &m_isPlacingStart; }
    bool *GetIsPlacingFinalPtr() { return &m_isPlacingFinal; }
    GridMode &GetGridModeRef() { return m_gridMode; }

private:
    void UpdateBoxMode();
    void UpdatePolygonMode();

    void DrawBoxGrid(Graphics &gfx);
    void DrawPolygonGrid(Graphics &gfx);

    void ComputeBoxGridNeighbors();
    void ComputePolygonGridNeighbors();

private:
    InputUtility &m_iu;
    class AStar &m_solver;

    //Box grid
    sf::Vector2i m_boxSize;
    sf::Vector2i m_size;
    std::vector<std::shared_ptr<TraverseBox>> m_boxGridLayout;

    //Polygon grid
    Voronoi *m_voronoi_creator;
    std::vector<VoronoiPoint *> m_voronoi_points;
    std::vector<VEdge> m_edges;
    std::vector<std::shared_ptr<TraversePolygon>> m_polygonGridLayout;

    bool m_isPlacingObstacles;
    bool m_isRemovingObstacles;
    bool m_isPlacingStart;
    bool m_isPlacingFinal;

    GridMode m_gridMode;
};