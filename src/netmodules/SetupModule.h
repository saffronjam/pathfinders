#pragma once

#include "INetModule.h"

class SetupModule : public INetModule
{
public:
    SetupModule(INetMgr *ioHelper);

    void HandlePacket(UID, const ParsedPacket &packet) override;
    void HandlePacket(UDPSync, const ParsedPacket &packet) override;
};