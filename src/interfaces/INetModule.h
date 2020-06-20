#pragma once

#include "IPacketHandler.h"

class INetMgr;

class INetModule : public IPacketHandler
{
public:
    INetModule(INetMgr *ioHelper);

protected:
    INetMgr *m_ioHelper;
};