#pragma once

#include <deque>
#include <thread>

#include <SFML/System/Time.hpp>
#include <SFML/System/Sleep.hpp>

#include "TraverseGrid.h"

class Pathfinder
{
protected:
    enum class State
    {
        Finding,
        WaitingForStart,
        Paused,
        Finished,
        BeingCollected
    };

public:
    Pathfinder();

    void DrawProgress();
    void DrawNeighbors();
    void DrawResult();

    void AssignNodes(const std::map<long, Node> &nodes) noexcept { m_nodes = nodes; }
    void SetSleepDelay(const sf::Time &delay) noexcept { m_sleepDelay = delay; }
    void SetTraverseGrid(const TraverseGrid *traverseGrid) noexcept { m_traverseGrid = traverseGrid; }

    void Start();
    void Pause();
    void Resume();
    void Restart();
    void Reset();

    void SetSleepDelay(sf::Time delay);

    bool IsDone() const noexcept { return m_state == State::Finished; }

protected:
    std::map<long, Node> &GetNodes() noexcept { return m_nodes; }
    virtual void FindPath() = 0;

    void PauseCheck();
    void SleepDelay();

private:
    void FindPathThreadFn();
    void OnFinish();
    void CollectFinder();

protected:
    State m_state;
    std::thread m_finder;
    const TraverseGrid *m_traverseGrid;

    sf::Time m_sleepDelay;
    bool m_minorDelay;
    sf::Int64 m_minorDelayTimer;

    bool m_pathWasFound;

private:
    std::map<long, Node> m_nodes;
    std::map<long, Node> m_nodesRestart;
    std::map<long, Node> m_nodesReset;

    std::set<const Node *> m_finalPath;
};