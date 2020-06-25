#include "GreedyBFS.h"

void GreedyBFS::FindPath(long startUID, long goalUID)
{
    m_checkingQueue.push_front(startUID);
    m_maxCost = -1.0f;
    GetNode(startUID).SetCost("Heuristic", vl::Length(GetNode(startUID).GetPosition() - GetNodes().at(goalUID).GetPosition()));
    while (m_checkingQueue.size() && m_state != State::BeingCollected)
    {
        PauseCheck();
        m_activeNodeUID = m_checkingQueue.front();
        Node &activeNode = GetNodes().at(m_activeNodeUID);
        if (m_activeNodeUID == goalUID)
        {
            // Algorithm is done
            m_checkingQueue.clear();
            m_maxCost = activeNode.GetCost("Heuristic");
        }
        else if (activeNode.GetCost("Heuristic") < m_maxCost || m_maxCost == -1.0f)
        {
            for (auto &neighborUID : activeNode.GetNeighbors())
            {
                Node &neighbor = GetNodes().at(neighborUID);
                if (m_state == State::BeingCollected)
                    break;
                SleepDelay();
                PauseCheck();
                if (!m_traverseGrid->IsObstacle(neighbor.GetUID()) && neighbor.GetUID() != activeNode.GetViaUID())
                {
                    neighbor.SetCost("Heuristic", vl::Length(neighbor.GetPosition() - GetNodes().at(goalUID).GetPosition()));

                    if (neighbor.GetCost("Heuristic") < activeNode.GetCost("Heuristic") && !neighbor.WasVisited())
                    {
                        neighbor.SetVia(m_activeNodeUID);
                        m_checkingQueue.push_front(neighborUID);
                        m_checkingQueue.push_front(m_activeNodeUID);
                    }
                }
            }
            m_checkingQueue.pop_front();
        }
        else
        {
            m_checkingQueue.pop_front();
        }
    };
    m_checkingQueue.clear();
}