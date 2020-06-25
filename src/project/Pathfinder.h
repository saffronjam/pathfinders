#pragma once

#include <deque>
#include <thread>

#include <SFML/System/Time.hpp>
#include <SFML/System/Sleep.hpp>

#include "TraverseGrid.h"

class Pathfinder
{
public:
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

    Pathfinder::State GetState() const noexcept { return m_state; }

    void AssignNodes(const std::map<long, Node> &nodes) noexcept;
    void SetTraverseGrid(const TraverseGrid *traverseGrid) noexcept { m_traverseGrid = traverseGrid; }
    void SetSleepDelay(sf::Time delay);

    void Start(long startUID, long goalUID, const std::vector<long> &subGoalsUIDs);
    void Pause();
    void Resume();
    void Restart();
    void Reset();

    bool IsDone() const noexcept { return m_state == State::Finished; }

    virtual const std::string &GetName() = 0;

protected:
    std::map<long, Node> &GetNodes() noexcept { return m_nodes; }
    Node &GetNode(long uid) { return GetNodes().at(uid); }
    virtual void FindPath(long startUID, long goalUID) = 0;

    void PauseCheck();
    void SleepDelay();

private:
    void FindPathThreadFn(long startUID, long goalUID, const std::vector<long> &subGoalsUIDs);
    bool CheckFindPathResult(long fromUID, long toUID);
    void AppendFinalPath(long startUID, long goalUID);
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
    std::vector<const Node *> m_finalPath;
    sf::Time m_finalPathTimer;
    int m_nFinalPathNodes;
};