#pragma once

#include "INetModule.h"

class PingModule : public INetModule
{
public:
    PingModule(INetMgr *ioHelper);

    void HandlePacket(AreYouAlive, const ParsedPacket &packet) override;
    void HandlePacket(IAmAlive, const ParsedPacket &packet) override;
};