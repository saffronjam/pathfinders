#pragma once

#include "Clock.h"
#include "NetUID.h"

class IConnInfo
{
public:
    IConnInfo(NetUID uid);
    bool operator<(const IConnInfo &rhs) const
    {
        return m_uid > rhs.m_uid;
    }
    NetUID GetUID() const noexcept { return m_uid; }
    virtual void Reset() { m_uid = 0; }

private:
    NetUID m_uid;
};