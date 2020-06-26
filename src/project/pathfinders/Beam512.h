#pragma once

#include "Pathfinder.h"

class Beam512 : public Pathfinder
{
public:
    void FindPath(long startUID, long goalUID) override;

    const std::string &GetName() override { return m_name; }

private:
    std::deque<long> m_checkingQueue;
    std::string m_name = "Beam 512";
    static constexpr int beamWidth = 512;
};