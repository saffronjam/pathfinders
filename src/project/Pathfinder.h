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
    ~Pathfinder();

    void DrawAnticipation();
    void DrawViaConnections();
    void DrawNeighbors();
    void DrawResult();

    void AssignNodes(const std::map<long, Node> &nodes) noexcept;
    void SetTraverseGrid(const TraverseGrid *traverseGrid) noexcept { m_traverseGrid = traverseGrid; }
    void SetSleepDelay(sf::Time delay);

    void Start();
    void Pause();
    void Resume();
    void Restart();
    void Reset();

    bool IsDone() const noexcept { return m_state == State::Finished; }

    virtual const std::string &GetName() = 0;

protected:
    std::map<long, Node> &GetNodes() noexcept { return m_nodes; }
    Node &GetNode(long uid) { return GetNodes().at(uid); };
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
    long m_activeNodeUID;

    sf::Time m_sleepDelay;
    bool m_minorDelay;
    sf::Int64 m_minorDelayTimer;

    bool m_pathWasFound;

private:
    std::map<long, Node> m_nodes;

    std::set<const Node *> m_finalPath;
};