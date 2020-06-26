#include "GreedyBFS.h"

void GreedyBFS::FindPath(long startUID, long goalUID)
{
    m_checkingQueue.push_front(startUID);
    GetNode(startUID).SetCost("Heuristic", vl::Length(GetNode(startUID).GetPosition() - GetNodes().at(goalUID).GetPosition()));
    while (m_checkingQueue.size() && m_state != State::BeingCollected)
    {
        PauseCheck();
        m_activeNodeUID = m_checkingQueue.front();
        if (m_activeNodeUID == goalUID)
            break;

        Node &activeNode = GetNodes().at(m_activeNodeUID);
        m_checkingQueue.pop_front();

        for (auto &neighborUID : activeNode.GetNeighbors())
        {
            if (m_state == State::BeingCollected)
                break;
            PauseCheck();
            SleepDelay();

            Node &neighbor = GetNodes().at(neighborUID);
            if (!m_traverseGrid->IsObstacle(neighborUID) && neighborUID != activeNode.GetViaUID())
            {
                neighbor.SetCost("Heuristic", vl::Length(neighbor.GetPosition() - GetNodes().at(goalUID).GetPosition()));
                if (!neighbor.WasVisited())
                {
                    m_checkingQueue.push_front(neighborUID);
                    neighbor.SetVia(m_activeNodeUID);
                }
            }
            activeNode.AddVisitedNeighbor(neighborUID);
        }
        std::sort(m_checkingQueue.begin(),
                  m_checkingQueue.end(),
                  [this](const auto &lhs, const auto &rhs) { return GetNode(lhs).GetCost("Heuristic") < GetNode(rhs).GetCost("Heuristic"); });
    }
    m_checkingQueue.clear();
}