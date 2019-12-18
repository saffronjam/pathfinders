#pragma once

#include <deque>
#include <set>
#include <memory>

#include "Graphics.hpp"
#include "Grid.hpp"

class AStar
{
public:
    AStar(Grid &m_grid);

    void Update(sf::Time dt);

    float &GetStepTimeRef() { return m_stepTime; }

    void DrawFinal(Graphics &gfx);
    void DrawCurrentAnticipation(Graphics &gfx);
    void DrawAllNodeViaConnections(Graphics &gfx);
    void DrawLineToNeighbors(Graphics &gfx);

    Node GetStart() { return *m_startNode; }
    Node GetFinal() { return *m_finalNode; }
    bool GetIsDone() { return m_isDone; }
    bool &GetIsPausedRef() { return m_isPaused; }

    void Solve();
    void Pause();
    void ResetAll();
    void ResetSearch();

private:
    void Step();
    void Reset();

    float CalculateHCost(std::shared_ptr<Node> to_calc);
    void ComputeFinalPath();
    std::shared_ptr<Node> FindClosestNode(sf::Vector2f const &position);

    void SetStart(std::shared_ptr<Node> startNode);
    void SetFinal(std::shared_ptr<Node> finalNode);

    void EnterNewMode(Grid::GridMode gridMode);

private:
    Grid &m_grid;
    Grid::GridMode &m_gridMode;

    std::shared_ptr<Node> m_startNode;
    std::shared_ptr<Node> m_finalNode;
    std::shared_ptr<Node> m_active;

    std::set<std::shared_ptr<Node>> m_allNodes;
    std::deque<std::shared_ptr<Node>> m_checkingQueue;

    std::set<std::shared_ptr<Node>> m_finalPath;

    sf::Vector2f m_defaultStartAimingPosition;
    sf::Vector2f m_defaultFinalAimingPosition;

    float m_stepTime;
    float m_currentWait;

    float m_maxCost;

    bool m_isPaused;
    bool m_isDone;
    bool m_noPathFound;

    friend Grid;
};